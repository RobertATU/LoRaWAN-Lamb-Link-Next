function Decode(fPort, bytes, variables) {
    var decoded = {};

  if((bytes[13]&0x80) !==0){
    decoded.accelero_x = (0xff<<24 |0xff<<16 |bytes[13] << 8 | bytes[14]);}
  else{decoded.accelero_x = (bytes[13] << 8 | bytes[14]);}
    
  if((bytes[21]&0x80) !==0){
    decoded.latitude = (0xff<<24 |bytes[21] << 16 | bytes[22] << 8 | bytes[23] ) / 10000.0;}
  else{ decoded.latitude = (bytes[21] << 16 | bytes[22] << 8 | bytes[23] ) / 10000.0;}
 
  if((bytes[24]&0x80) !==0){
    decoded.longitude = (0xff<<24 | bytes[24] << 16 | bytes[25] << 8 | bytes[26]) / 10000.0;}
  else{decoded.longitude =(bytes[24] << 16 | bytes[25] << 8 | bytes[26]) / 10000.0;}

    decoded.sats = (bytes[38] << 8 | bytes[39]);
    decoded.name = "ATU";

    return decoded;
}