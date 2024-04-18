package ie.atu.springbackend;
import com.twilio.Twilio;
import com.twilio.rest.api.v2010.account.Message;
import io.github.cdimascio.dotenv.Dotenv;
import org.json.JSONObject;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;
import org.springframework.transaction.annotation.Transactional;
import java.text.SimpleDateFormat;
import java.util.Calendar;
import java.util.List;
import java.util.Random;
import java.util.TimeZone;

@Service
public class PinsService {
    public static final String ACCOUNT_SID = Dotenv.load().get("ACCOUNT_SID");
    public static final String AUTH_TOKEN = Dotenv.load().get("AUTH_TOKEN");
    public int count;
    @Autowired
    private PinsRepo pinsRepo;
    public List<Pins>allPins(){
        List<Pins> pinsList = pinsRepo.findAll();
        SimpleDateFormat outputFormat = new SimpleDateFormat("dd/MM/yyyy, HH:mm:ss");

        Calendar calendar = Calendar.getInstance();

        TimeZone irelandTime = TimeZone.getTimeZone("Europe/Dublin");

        boolean isDaylightSaving = irelandTime.inDaylightTime(calendar.getTime());
            for(Pins pins : pinsList) {
                if(isDaylightSaving){
                    calendar.setTime(pins.getId().getDate());
                    calendar.add(Calendar.HOUR_OF_DAY,1);
                    pins.setDate(outputFormat.format(calendar.getTime()));
                }

        else {
                    pins.setDate(outputFormat.format(pins.getId().getDate()));
                }
            }
        return pinsList;
    }



    public Pins addPins(Pins pins) {
        String jsonString = pins.getObjectJSON();
        JSONObject json = new JSONObject(jsonString);
        pins.setLongitude(json.getDouble("longitude"));
        pins.setLatitude(json.getDouble("latitude"));
        pins.setSheepId(json.getString("name"));
        pins.setAccelero_x(json.getDouble("accelero_x"));
        pins.setGenId(String.valueOf(new Random().nextInt(1000000)));
        if(pins.getAccelero_x() < 0){
            count++;
            System.out.println("Count" + count);
        }
        else if(pins.getAccelero_x() > 0 &&count > 1){
            Twilio.init(ACCOUNT_SID, AUTH_TOKEN);
            Message message = Message.creator(
                    new com.twilio.type.PhoneNumber("whatsapp:+353877178072"),
                    new com.twilio.type.PhoneNumber("whatsapp:+14155238886"),
                    "Your Sheep: "+pins.getSheepId()+" is back up. View Location: https://lo-ra-wan-lamb-link-next.vercel.app/").create();
                    System.out.println(message.getSid());

            count = 0;
            System.out.println("Count" + count);
        }
        else {

            count = 0;
            System.out.println("Count" + count);
        }

        if(count == 2){
            Twilio.init(ACCOUNT_SID, AUTH_TOKEN);
            Message message = Message.creator(
                    new com.twilio.type.PhoneNumber("whatsapp:+353877178072"),
                    new com.twilio.type.PhoneNumber("whatsapp:+14155238886"),
                    "Your Sheep: "+pins.getSheepId()+ " is in need of assistance. View Location: https://lo-ra-wan-lamb-link-next.vercel.app/").create();
                    System.out.println(message.getSid());

        }
        System.out.println(pins);
        return pinsRepo.save(pins);
    }

    @Transactional
    public Pins deletePin(String genId) {
        return pinsRepo.deletePinsByGenId(genId);
    }

    public void deleteAllPin() {
         pinsRepo.deleteAll();
    }


}
