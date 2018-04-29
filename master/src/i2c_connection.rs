use rustpi_io::i2c::I2CPi;
use std::{thread, time};
use std::io::Result;

#[allow(dead_code)]
#[derive(Debug, Copy, Clone)]
pub enum ArduinoPin {
    D0 = 0,
    D1 = 1,
    D2 = 2,
    D3 = 3,
    D4 = 4,
    D5 = 5,
    D6 = 6,
    D7 = 7,
    D8 = 8,
    D9 = 9,
    D10 = 10,
    D11 = 11,
    D12 = 12,
    D13 = 13,
    A0 = 14,
    A1 = 15,
    A2 = 16,
    A3 = 17,
    A4 = 18,
    A5 = 19,
    A6 = 20,
    A7 = 21,
}

pub struct Bus {
    smbus: I2CPi,
}

impl Bus {
    pub fn new(slave_address: u16) -> Result<Self> {
        let smb = I2CPi::new(slave_address)?;
        Ok(Bus { smbus: smb })
    }
    #[allow(dead_code)]
    pub fn change_slave_address(&mut self, slave_address: u16) -> Result<()> {
        Ok(self.smbus.change_slave_address(slave_address)?)
    }
    pub fn read_word(&mut self, pin: ArduinoPin) -> Result<u16> {
        self.smbus.write_byte(0x01, pin as u8)?;
        thread::sleep(time::Duration::from_millis(100));
        Ok(self.smbus.read_word(0x00)?)
    }
}
