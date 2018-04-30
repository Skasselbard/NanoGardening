extern crate rustpi_io;
#[macro_use]
extern crate clap;

mod device;
mod i2c_connection;

use clap::App;
use rustpi_io::pi::get_raspberry_info;
use i2c_connection::{ArduinoPin, Bus};
use device::humidity_sensor::HumiditySensor;
use device::valve::Valve;
use std::{thread, time};

fn main() {
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
    if let Some(matches) = matches.subcommand_matches("test") {
        if matches.is_present("debug") {
            println!("Printing debug info...");
        } else {
            println!("Printing normally...");
        }
    }

    let mut bus = Bus::new(0x08).unwrap();
    let mut humidity_sensor = HumiditySensor::new(ArduinoPin::A0, &mut bus);
    let mut valve = Valve::new(14);
    println!("{:?}", get_raspberry_info().unwrap());
    loop {
        println!("{:?}", humidity_sensor.read().unwrap());
        thread::sleep(time::Duration::from_millis(10000));
    }
}
