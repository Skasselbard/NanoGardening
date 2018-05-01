extern crate rustpi_io;
#[macro_use]
extern crate clap;

mod device;
mod i2c_connection;

use clap::App;
use rustpi_io::pi::get_raspberry_info;
use i2c_connection::{ArduinoPin, Bus};
use device::{Device, humidity_sensor::HumiditySensor, valve::Valve};
use std::{thread, time};
use std::collections::HashMap;
use std::sync::Arc;

fn init_devices<'a>(bus: Arc<Bus>) -> HashMap<String, Box<Device>> {
    let mut device_list: HashMap<String, Box<Device>> = HashMap::new();
    device_list.insert(
        "hA0".to_string(),
        Box::new(HumiditySensor::new(ArduinoPin::A0, bus.clone())),
    );
    device_list.insert(
        "hA1".to_string(),
        Box::new(HumiditySensor::new(ArduinoPin::A1, bus.clone())),
    );

    device_list.insert("v14".to_string(), Box::new(Valve::new(14).unwrap()));

    device_list
}

fn main() {
    let bus = Arc::new(Bus::new(0x08).unwrap());
    let device_list = init_devices(bus);

    let yaml = load_yaml!("cli/cli.yml");
    let matches = App::from_yaml(yaml).get_matches();

    // Gets a value for config if supplied by user, or defaults to "default.conf"
    let config = matches.value_of("config").unwrap_or("default.conf");
    println!("Value for config: {}", config);

    // Calling .unwrap() is safe here because "INPUT" is required (if "INPUT" wasn't
    // required we could have used an 'if let' to conditionally get the value)
    println!("Using input file: {}", matches.value_of("INPUT").unwrap());

    // Vary the output based on how many times the user used the "verbose" flag
    // (i.e. 'myprog -v -v -v' or 'myprog -vvv' vs 'myprog -v'
    match matches.occurrences_of("v") {
        0 => println!("No verbose info"),
        1 => println!("Some verbose info"),
        2 => println!("Tons of verbose info"),
        3 | _ => println!("Don't be crazy"),
    }

    // You can handle information about subcommands by requesting their matches by name
    // (as below), requesting just the name used, or both at the same time
    if let Some(matches) = matches.subcommand_matches("humidity") {
        let id = matches.value_of("id").unwrap();
    }

    println!("{:?}", get_raspberry_info().unwrap());
    loop {
        //println!("{:?}", humidity_sensor.read().unwrap());
        thread::sleep(time::Duration::from_millis(10000));
    }
}
