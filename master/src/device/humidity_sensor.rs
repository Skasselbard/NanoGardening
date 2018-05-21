use device::{Device, DeviceName, Sensor};
use i2c_connection::{ArduinoPin, Bus};
use std::io::{Error, ErrorKind, Result};
use std::sync::Arc;

pub struct HumiditySensor {
    name: DeviceName,
    pin: ArduinoPin,
    bus: Arc<Bus>,
    reading_count: u16,
}

impl HumiditySensor {
    pub fn new(pin: ArduinoPin, bus: Arc<Bus>) -> Self {
        HumiditySensor {
            name: DeviceName::new(),
            pin: pin,
            bus: bus,
            reading_count: 10,
        }
    }
}

impl Device for HumiditySensor {
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

impl Sensor for HumiditySensor {
    fn read(&mut self) -> Result<u16> {
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

fn smooth_readings(readings: Vec<u16>) -> u16 {
    let mut average = 0;
    for i in 0..readings.len() {
        average += readings[i];
    }
    average / (readings.len() as u16)
}
