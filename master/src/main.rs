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

enum ExitCode {
    OK = 0,
    Error = 1,
}

fn run() -> ExitCode {
    let yaml = load_yaml!("cli/cli.yml");
    let matches = App::from_yaml(yaml).get_matches();

    let bus = Rc::new(RefCell::new(Bus::new(0x08).unwrap()));
    let mut device_list = init_devices(bus);

    // Gets a value for config if supplied by user, or defaults to "default.conf"
    if matches.is_present("printDevices") {
        print_devices(&device_list);
    }

    if let Some(matches) = matches.subcommand_matches("sensor") {
        let id = matches.value_of("id").unwrap();
        if let Some(DeviceTypes::Sensor(sensor)) = get_device(&mut device_list, id) {
            match sensor.read() {
                Ok(value) => println!("{}", value),
                Err(error) => {
                    eprintln!("{}", error);
                    return ExitCode::Error;
                }
            }
        } else {
            eprintln!("Device is not a sensor");
        }
    }

    if let Some(matches) = matches.subcommand_matches("switch") {
        if let Some(id) = matches.value_of("open") {
            if let Some(DeviceTypes::Switch(switch)) = get_device(&mut device_list, id) {
                match switch.open() {
                    Ok(_) => {}
                    Err(error) => {
                        eprintln!("{}", error);
                        return ExitCode::Error;
                    }
                }
            }
        } else {
            if let Some(id) = matches.value_of("close") {
                if let Some(DeviceTypes::Switch(switch)) = get_device(&mut device_list, id) {
                    match switch.close() {
                        Ok(_) => {}
                        Err(error) => {
                            eprintln!("{}", error);
                            return ExitCode::Error;
                        }
                    }
                }
            } else {
                if let Some(mut params) = matches.values_of("toggleTimed") {
                    use std::thread;
                    use std::time::Duration;
                    let id = match params.next() {
                        Some(id) => id,
                        None => {
                            eprintln!("No parameters found for toggleTimed");
                            return ExitCode::Error;
                        }
                    };
                    let seconds = match params.next() {
                        Some(time) => {
                            if let Ok(seconds) = time.parse::<u64>() {
                                Duration::new(seconds, 0)
                            } else {
                                eprintln!("Could not parse duration");
                                return ExitCode::Error;
                            }
                        }
                        None => {
                            eprintln!("No parameters found for toggleTimed");
                            return ExitCode::Error;
                        }
                    };
                    if let Some(DeviceTypes::Switch(switch)) = get_device(&mut device_list, id) {
                        match switch.open() {
                            Ok(_) => {}
                            Err(error) => {
                                eprintln!("{}", error);
                                return ExitCode::Error;
                            }
                        }
                        thread::sleep(seconds);
                        for _ in 0..100 {
                            match switch.close() {
                                Ok(_) => return ExitCode::OK,
                                Err(_) => {
                                    thread::sleep(Duration::from_millis(100));
                                }
                            }
                            eprintln!("Unable to switch back");
                            return ExitCode::Error;
                        }
                    }
                }
            }
        }
    }
    ExitCode::OK
}

fn init_devices<'a>(bus: Rc<RefCell<Bus>>) -> Vec<DeviceTypes> {
    let mut device_list: Vec<DeviceTypes> = Vec::with_capacity(3);
    let mut dev1 = HumiditySensor::new(ArduinoPin::A0, Rc::clone(&bus));
    let mut dev2 = HumiditySensor::new(ArduinoPin::A1, Rc::clone(&bus));
    let mut dev3 = Valve::new(17).unwrap();
    let mut dev4 = Valve::new(4).unwrap();
    dev1.set_id("humidityA0".to_string());
    dev2.set_id("humidityA1".to_string());
    dev3.set_id("Valve17".to_string());
    dev4.set_id("Valve4".to_string());
    device_list.push(DeviceTypes::Sensor(Box::new(dev1)));
    device_list.push(DeviceTypes::Sensor(Box::new(dev2)));
    device_list.push(DeviceTypes::Switch(Box::new(dev3)));
    device_list.push(DeviceTypes::Switch(Box::new(dev4)));
    device_list
}

fn print_devices(devices: &Vec<DeviceTypes>) {
    println!("ID\tType",);
    for device in devices {
        let (id, typ) = match device {
            DeviceTypes::Sensor(sensor) => (sensor.get_id(), "HumiditySensor"),
            DeviceTypes::Switch(switch) => (switch.get_id(), "Switch"),
        };
        println!("{}\t{}", id, typ)
    }
}

fn get_device<'a>(devices: &'a mut Vec<DeviceTypes>, id: &str) -> Option<&'a mut DeviceTypes> {
    if let Some(index) = devices.iter_mut().position(|device| match device {
        DeviceTypes::Sensor(sensor) => sensor.get_id() == id,
        DeviceTypes::Switch(switch) => switch.get_id() == id,
    }) {
        if let Some(device_types) = devices.get_mut(index) {
            Some(device_types)
        } else {
            None
        }
    } else {
        eprintln!("Device not found");
        None
    }
}

fn main() {
    let exit_code = run();
    std::process::exit(exit_code as i32);
}
