function Decoder(bytes, port) {
  // Decode an uplink message from a buffer
  // (array) of bytes to an object of fields.
  var decoded = {};
  
  decoded.counter = (bytes[0] << 8) | (bytes[1]);
  decoded.interval =  bytes[2];
  decoded.temp =  (bytes[3] / 10) + 5;
  
  return decoded;
}