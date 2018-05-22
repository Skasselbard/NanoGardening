extern crate rand;
extern crate rustpi_io;
#[macro_use]
extern crate clap;

mod device;
mod i2c_connection;

use clap::App;
use device::{Device, Sensor, Switch, humidity_sensor::HumiditySensor, valve::Valve};
use i2c_connection::{ArduinoPin, Bus};
use std::cell::RefCell;
use std::rc::Rc;

enum DeviceTypes {
    Sensor(Box<Sensor>),
    Switch(Box<Switch>),
}

fn init_devices<'a>(bus: Rc<RefCell<Bus>>) -> Vec<DeviceTypes> {
    let mut device_list: Vec<DeviceTypes> = Vec::with_capacity(3);
    let mut dev1 = HumiditySensor::new(ArduinoPin::A0, Rc::clone(&bus));
    let mut dev2 = HumiditySensor::new(ArduinoPin::A1, Rc::clone(&bus));
    let mut dev3 = Valve::new(14).unwrap();
    dev1.set_id("humidityA0".to_string());
    dev2.set_id("humidityA1".to_string());
    dev3.set_id("Valve14".to_string());
    device_list.push(DeviceTypes::Sensor(Box::new(dev1)));
    device_list.push(DeviceTypes::Sensor(Box::new(dev2)));
    device_list.push(DeviceTypes::Switch(Box::new(dev3)));
    device_list
}

fn devices_to_string(devices: &Vec<DeviceTypes>) -> String {
    "devices".to_string()
}

fn main() {
    let yaml = load_yaml!("cli/cli.yml");
    let matches = App::from_yaml(yaml).get_matches();

    let bus = Rc::new(RefCell::new(Bus::new(0x08).unwrap()));
    let mut device_list = init_devices(bus);

    // Gets a value for config if supplied by user, or defaults to "default.conf"
    if matches.is_present("printDevices") {
        println!("{}", devices_to_string(&device_list));
    }

    if let Some(matches) = matches.subcommand_matches("sensor") {
        let id = matches.value_of("id").unwrap();
        if let Some(index) = device_list.iter_mut().position(|device| match device {
            DeviceTypes::Sensor(sensor) => sensor.get_id() == id,
            _ => false,
        }) {
            if let Some(DeviceTypes::Sensor(sensor)) = device_list.get_mut(index) {
                match sensor.read() {
                    Ok(value) => println!("{}", value),
                    Err(error) => eprintln!("{}", error),
                }
            }
        } else {
            eprintln!("Device not found or it is not a sensor");
        }
    }
    //println!("{:?}", get_raspberry_info().unwrap());
}
