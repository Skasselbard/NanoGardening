extern crate rustpi_io;
use rustpi_io::serial::*;
use rustpi_io::pi::*;
//use rustpi_io::gpio::*;
use std::string::String;
use std::{thread, time};
use std::io::{BufRead, Read, Write};
#[derive(Debug)]
enum Control {
    Null = 0,                 // empty Data
    StartOfHeading = 1,       // first header character
    StartOfText = 2,          // first message character and last header character
    EndOfText = 3,            // interrupt message
    EndOfTransmission = 4,    // end message
    Enquiry = 5,              // ask for next message
    Acknowledge = 6,          // no problem occurred
    NegativeAcknowledge = 21, // problem detected
}

fn build_message(packetNumber: u32, pin: u8) -> Vec<u8> {
    let mut message = Vec::with_capacity(11);
    let packet_number_bytes: [u8; 4] = unsafe { std::mem::transmute_copy(&packetNumber) };
    message.push(Control::StartOfHeading as u8);
    // for i in (0..4).rev(){// somehow the packet number is reversed
    //     message.push(packet_number_bytes[i])
    // }
    message.extend_from_slice(&packet_number_bytes);
    message.push(pin as u8);
    message.push(0 as u8); //read
    message.push(0 as u8); // write value
    message.push(Control::StartOfText as u8);
    message.push(Control::EndOfText as u8);
    message.push(Control::EndOfTransmission as u8);
    message
}

fn main() {
    println!("{:?}", get_raspberry_info().unwrap());
    let mut spi = SerialPi::new(
        Device::CE0,
        Speed::Hz7629,
        SpiMode::Mode0,
        ComMode::FullDuplex,
    ).unwrap();
    let mut packet_number: u32 = 0;
    loop {
        let tx_buf: Vec<u8> = build_message(packet_number, 14);
        println!("Transfer: {:?}", tx_buf);
        spi.write(tx_buf.as_slice()).unwrap();
        let mut received_length;
        {
            let received = spi.fill_buf().unwrap();
            received_length = received.len();
            println!("Receive: {:?}", received);
        }
        spi.consume(received_length);
        packet_number = packet_number + 1;
        thread::sleep(time::Duration::from_millis(2000));
    }
}
