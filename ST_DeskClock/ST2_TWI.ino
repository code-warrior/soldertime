//***********************************************************************************
//                                                          I2C TX RX
//***********************************************************************************

// Transmit I2C Data
void transmit(byte device, byte regadd, byte tx_data)
{
   Wire.beginTransmission(device);
   Wire.write(regadd);
   Wire.write(tx_data);
   Wire.endTransmission();
}

// Receive I2C Data
void receive(byte devicerx, byte regaddrx)
{
   Wire.beginTransmission(devicerx);
   Wire.write(regaddrx);
   Wire.endTransmission();
   Wire.requestFrom(int(devicerx), 1);

   byte c = 0;

   if (Wire.available()) {
      data_received_on_i2c = Wire.read();
   }
}
