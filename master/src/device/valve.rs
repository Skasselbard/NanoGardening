use device::{Device, DeviceName, Switch};
use rustpi_io::gpio::{GPIOData::{High, Low}, GPIOMode::Write, GPIO};
use std::any::Any;
use std::io::Result;

pub struct Valve {
    name: DeviceName,
    gpio: GPIO,
}

impl Valve {
    pub fn new(pin: u8) -> Result<Self> {
        Ok(Valve {
            name: DeviceName::new(),
            gpio: GPIO::new(pin, Write)?,
        })
    }
}

impl Device for Valve {
    fn name(&mut self) -> &mut DeviceName {
        &mut self.name
    }

    fn set_id(&mut self, id: String) {
        self.name.set_id(id)
    }

    fn get_id(&self) -> &str {
        self.name.get_id()
    }
}

impl Switch for Valve {
    fn is_open(&mut self) -> Result<(bool)> {
        if self.gpio.value()? == High {
            Ok(true)
        } else {
            Ok(false)
        }
    }

    fn open(&mut self) -> Result<()> {
        Ok(self.gpio.set(High)?)
    }

    fn close(&mut self) -> Result<()> {
        Ok(self.gpio.set(Low)?)
    }
}
