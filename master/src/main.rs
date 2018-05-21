extern crate rand;
extern crate rustpi_io;
#[macro_use]
extern crate clap;
#[macro_use]
extern crate downcast_rs;

mod device;
mod i2c_connection;

use clap::App;
use device::{Device, Sensor, Switch, humidity_sensor::HumiditySensor, valve::Valve};
use i2c_connection::{ArduinoPin, Bus};
use rustpi_io::pi::get_raspberry_info;
use std::sync::{Arc, Weak};
use std::{thread, time};

fn init_devices<'a>(bus: Weak<Bus>) -> Vec<Box<Device>> {
    let mut device_list: Vec<Box<Device>> = Vec::with_capacity(3);
    let mut dev1 = HumiditySensor::new(ArduinoPin::A0, bus.clone());
    let mut dev2 = HumiditySensor::new(ArduinoPin::A1, bus.clone());
    let mut dev3 = Valve::new(14).unwrap();
    dev1.set_id("humidityA0".to_string());
    dev2.set_id("humidityA1".to_string());
    dev3.set_id("Valve14".to_string());
    device_list.push(Box::new(dev1));
    device_list.push(Box::new(dev2));
    device_list.push(Box::new(dev3));
    device_list
}

fn devices_to_string(devices: &Vec<Box<Device>>) -> String {
    "devices".to_string()
}

fn main() {
    let yaml = load_yaml!("cli/cli.yml");
    let matches = App::from_yaml(yaml).get_matches();

    let bus = Arc::new(Bus::new(0x08).unwrap());
    let mut device_list = init_devices(Arc::downgrade(&bus));

    // Gets a value for config if supplied by user, or defaults to "default.conf"
    if matches.is_present("printDevices") {
        println!("{}", devices_to_string(&device_list));
    }

    if let Some(matches) = matches.subcommand_matches("sensor") {
        let id = matches.value_of("id").unwrap();
        if let Some(index) = device_list
            .iter_mut()
            .position(|device| device.get_id() == id)
        {
            if let Some(device) = device_list.get_mut(index) {
                if let Some(sensor) = device.downcast_mut::<HumiditySensor>() {
                    println!("{:?}", sensor.read());
                    return;
                }
            }
        }
        eprintln!("Device not found or it is not a sensor");
    }
    //println!("{:?}", get_raspberry_info().unwrap());
}
