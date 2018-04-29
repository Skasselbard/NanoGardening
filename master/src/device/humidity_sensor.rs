use i2c_connection::{ArduinoPin, Bus};
use std::io::Result;

pub struct HumiditySensor<'a> {
    pin: ArduinoPin,
    bus: &'a mut Bus,
    reading_count: u16,
}

impl<'a> HumiditySensor<'a> {
    pub fn new(pin: ArduinoPin, bus: &'a mut Bus) -> Self {
        HumiditySensor {
            pin: pin,
            bus: bus,
            reading_count: 10,
        }
    }

    pub fn read(&mut self) -> Result<u16> {
        let mut readings = Vec::with_capacity(self.reading_count as usize);
        for _ in 0..self.reading_count {
            readings.push(self.bus.read_word(self.pin)?);
        }
        Ok(smooth_readings(readings))
    }
}

fn smooth_readings(readings: Vec<u16>) -> u16 {
    let mut average = 0;
    for i in 0..readings.len() {
        average += readings[i];
    }
    average / (readings.len() as u16)
}
