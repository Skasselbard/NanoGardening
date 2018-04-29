extern crate rustpi_io;
//use rustpi_io::serial::*;
use rustpi_io::pi::*;
use rustpi_io::i2c::*;
//use rustpi_io::gpio::*;
use std::{thread, time};
use std::time::Duration;

fn main() {
    let mut smb = SMBusPi::new().unwrap();
    smb.slave_address(0x08, false).unwrap();
    smb.set_timeout(Duration::from_millis(100)).unwrap();
    println!("{:?}", get_raspberry_info().unwrap());
    // println!("{:?}", smb.functionality());
    // println!("{:?}", smb.transfer_flags());
    loop {
        match smb.write_byte(0x01, 14) {
            Ok(_) => println!("send"),
            Err(error) => println!("error: {}", error),
        }
        thread::sleep(time::Duration::from_millis(1000));
        match smb.read_word(0x00) {
            Ok(word) => println!("received: {:x}", word),
            Err(error) => println!("error: {}", error),
        }
        thread::sleep(time::Duration::from_millis(1000));
    }
}
