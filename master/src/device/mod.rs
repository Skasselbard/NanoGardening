pub mod humidity_sensor;
pub mod valve;

use std::io::Result;

pub struct DeviceName {
    id: String,
}

impl DeviceName {
    pub fn new() -> Self {
        DeviceName {
            id: Self::random_name(),
        }
    }

    pub fn set_id(&mut self, id: String) {
        self.id = id;
    }

    pub fn get_id(&self) -> &str {
        &self.id
    }

    fn random_name() -> String {
        use rand::{thread_rng, Rng};
        thread_rng().gen_ascii_chars().take(32).collect()
    }
}

pub trait Device {
    fn name(&mut self) -> &mut DeviceName;
    fn set_id(&mut self, id: String);
    fn get_id(&self) -> &str;
}

pub trait Sensor: Device {
    fn read(&mut self) -> Result<u16>;
}

pub trait Switch: Device {
    fn is_open(&mut self) -> Result<bool>;
    fn open(&mut self) -> Result<()>;
    fn close(&mut self) -> Result<()>;
    fn toggle(&mut self) -> Result<()> {
        if self.is_open()? {
            self.close()
        } else {
            self.open()
        }
    }
}
