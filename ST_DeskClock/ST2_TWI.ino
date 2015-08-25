//*******************************************************************************************************************
//                                                          I2C TX RX
//*******************************************************************************************************************
void transmit(byte device, byte regadd, byte tx_data)                              // Transmit I2C Data
{
  Wire.beginTransmission(device);
  Wire.write(regadd);
  Wire.write(tx_data);
  Wire.endTransmission();
}

void receive(byte devicerx, byte regaddrx)                                       // Receive I2C Data
{
  Wire.beginTransmission(devicerx);
  Wire.write(regaddrx);
  Wire.endTransmission();
  Wire.requestFrom(int(devicerx), 1);

  byte c = 0;
  if(Wire.available())
  {
    data_received_on_i2c = Wire.read();
  }
}
