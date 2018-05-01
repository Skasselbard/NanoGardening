use i2c_connection::{ArduinoPin, Bus};
use std::io::{Error, ErrorKind, Result};
use Device;
use std::sync::Arc;

pub struct HumiditySensor {
    pin: ArduinoPin,
    bus: Arc<Bus>,
    reading_count: u16,
}

impl HumiditySensor {
    pub fn new(pin: ArduinoPin, bus: Arc<Bus>) -> Self {
        HumiditySensor {
            pin: pin,
            bus: bus,
            reading_count: 10,
        }
    }

    pub fn read(&mut self) -> Result<u16> {
        let mut readings = Vec::with_capacity(self.reading_count as usize);
        if let Some(bus) = Arc::get_mut(&mut self.bus) {
            for _ in 0..self.reading_count {
                readings.push(bus.read_word(self.pin)?);
            }
            Ok(smooth_readings(readings))
        } else {
            Err(Error::new(ErrorKind::PermissionDenied, "Bus in use"))
        }
    }
}

impl Device for HumiditySensor {}

fn smooth_readings(readings: Vec<u16>) -> u16 {
    let mut average = 0;
    for i in 0..readings.len() {
        average += readings[i];
    }
    average / (readings.len() as u16)
}
