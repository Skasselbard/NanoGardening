extern crate rustpi_io;
use rustpi_io::serial::*;
use rustpi_io::pi::*;
use rustpi_io::i2c::*;
//use rustpi_io::gpio::*;
use std::io;
use std::{mem, thread, time};
use std::io::{BufRead, Error, ErrorKind, Read, Write};

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

#[derive(Debug)]
struct Header {
    packetNumber: u32,
    pin: u8,
    ioType: u8, // 1 = In = write; 0 = Out = read;
    writeValue: u8,
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

fn parse_message(message: &Vec<u8>) -> io::Result<(Header, Vec<u8>)> {
    if let Some(start_of_heading) = message
        .iter()
        .position(|&x| x == (Control::StartOfHeading as u8))
    {
        let header = Header {
            packetNumber: message[start_of_heading + 1] as u32,
            pin: message[start_of_heading + mem::size_of::<u32>() + 1],
            ioType: message[start_of_heading + mem::size_of::<u32>() + 2],
            writeValue: message[start_of_heading + mem::size_of::<u32>() + 3],
        };
        let body_start = start_of_heading + mem::size_of::<Header>();
        if message[body_start] == (Control::StartOfText as u8) {
            let (_, rest) = message.as_slice().split_at(body_start + 1);
            if let Some(body_end) = rest.iter().position(|&x| x == (Control::EndOfText as u8)) {
                let mut body = Vec::new();
                let (body_slice, _) = rest.split_at(body_end);
                body.extend_from_slice(body_slice);
                Ok((header, body))
            } else {
                Err(Error::new(
                    ErrorKind::InvalidData,
                    "Unable to parse message body. EndOfText character not found",
                ))
            }
        } else {
            Err(Error::new(
                ErrorKind::InvalidData,
                "Unable to parse message body. StartOfText character not found",
            ))
        }
    } else {
        Err(Error::new(
            ErrorKind::InvalidData,
            "Unable to find message start. StartOfHeading character not found",
        ))
    }
}

fn main() {
    let mut i2c = I2CPi::new().unwrap();
    i2c.set_slave_address(0x01, false).unwrap();
    println!("{:?}", get_raspberry_info().unwrap());
    loop {
        // let tx_buf: Vec<u8> = Vec::new();
        // tx_buf.push(14 as u8);
        let mut rx_buf: Vec<u8> = Vec::new();
        thread::sleep(time::Duration::from_millis(2000));
        match i2c.i2c_write_block_data(0x02, &[14 as u8]) {
            Ok(_) => println!("send: {:?}", rx_buf),
            Err(error) => println!("error: {}", error),
        }
        match i2c.i2c_read_block_data(0x02, rx_buf.as_mut_slice()) {
            Ok(_) => println!("received: {:?}", rx_buf),
            Err(error) => println!("error: {}", error),
        }
    }
}

// fn main() {
//     println!("{:?}", get_raspberry_info().unwrap());
//     let mut spi = SerialPi::new(
//         Device::CE0,
//         Speed::Hz7629,
//         SpiMode::Mode0,
//         ComMode::FullDuplex,
//     ).unwrap();
//     let mut packet_number: u32 = 0;
//     loop {
//         let tx_buf: Vec<u8> = build_message(packet_number, 14);
//         let mut rx_buf: Vec<u8> = Vec::new();
//         println!("Transfer: {:?}", tx_buf);
//         spi.write(tx_buf.as_slice()).unwrap();
//         //thread::sleep(time::Duration::from_millis(2000));
//         loop {
//             match spi.read_until(Control::StartOfHeading as u8, &mut rx_buf) {
//                 Ok(n) => {
//                     rx_buf.drain(0..n - 1);
//                     break;
//                 }
//                 Err(ref error) if error.kind() == ErrorKind::Other => {
//                     thread::sleep(time::Duration::from_millis(2000));
//                     continue;
//                 }
//                 Err(error) => {
//                     println!("Error: {:?}", error);
//                     break;
//                 }
//             }
//         }
//         spi.read_until(Control::StartOfText as u8, &mut rx_buf);
//         spi.read_until(Control::EndOfText as u8, &mut rx_buf);
//         spi.read_until(Control::EndOfTransmission as u8, &mut rx_buf);
//         // let mut received_length;
//         // {
//         //     rx_buf.extend_from_slice(spi.fill_buf().unwrap());
//         //     received_length = rx_buf.len();
//         // }
//         // spi.consume(received_length);
//         //println!("Receive: {:?}", rx_buf);
//         packet_number = packet_number + 1;
//         if let Ok((header, body)) = parse_message(&rx_buf) {
//             println!("header: {:?}; body: {:?}", header, body);
//         } else {
//             println!("Unable to parse message");
//         }
//         thread::sleep(time::Duration::from_millis(1000));
//     }
// }
