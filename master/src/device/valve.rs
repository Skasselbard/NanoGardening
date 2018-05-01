use rustpi_io::gpio::{GPIOData::{High, Low}, GPIOMode::Write, GPIO};
use std::io::Result;
use Device;

pub struct Valve {
    gpio: GPIO,
}

impl Valve {
    pub fn new(pin: u8) -> Result<Self> {
        Ok(Valve {
            gpio: GPIO::new(pin, Write)?,
        })
    }

    pub fn toggle(&mut self) -> Result<()> {
        let curent_value = self.gpio.value()?;
        if curent_value == High {
            Ok(self.gpio.set(Low)?)
        } else {
            Ok(self.gpio.set(High)?)
        }
    }

    pub fn open(&mut self) -> Result<()> {
        Ok(self.gpio.set(High)?)
    }

    pub fn close(&mut self) -> Result<()> {
        Ok(self.gpio.set(Low)?)
    }
}

impl Device for Valve {}
