extern crate rustpi_io;

mod device;
mod i2c_connection;

use rustpi_io::pi::get_raspberry_info;
use i2c_connection::{ArduinoPin, Bus};
use device::humidity_sensor::HumiditySensor;
//use rustpi_io::gpio::*;
use std::{thread, time};

fn main() {
    let mut bus = Bus::new(0x08).unwrap();
    let mut humidity_sensor = HumiditySensor::new(ArduinoPin::A0, &mut bus);
    println!("{:?}", get_raspberry_info().unwrap());
    loop {
        println!("{:?}", humidity_sensor.read().unwrap());
        thread::sleep(time::Duration::from_millis(10000));
    }
}
