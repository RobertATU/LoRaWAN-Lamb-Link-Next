package ie.atu.springbackend;

import com.twilio.Twilio;
import com.twilio.rest.api.v2010.account.Message;
import org.json.JSONObject;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;
import org.springframework.transaction.annotation.Transactional;
import org.springframework.web.bind.annotation.GetMapping;

import java.text.SimpleDateFormat;
import java.util.List;
import java.util.Optional;
import java.util.Date;
import java.util.List;
import java.util.Optional;
import java.text.SimpleDateFormat;

import com.twilio.Twilio;
import com.twilio.converter.Promoter;
import com.twilio.rest.api.v2010.account.Message;
import com.twilio.type.PhoneNumber;

import java.net.URI;
import java.math.BigDecimal;

@Service
public class PinsService {
    // Find your Account Sid and Token at twilio.com/console
    public static final String ACCOUNT_SID = "";
    public static final String AUTH_TOKEN = "";
    public int count, upsideDown;
    @Autowired
    private PinsRepo pinsRepo;
    public List<Pins>allPins(){
        List<Pins> pinsList = pinsRepo.findAll();
        SimpleDateFormat outputFormat = new SimpleDateFormat("dd/MM/yyyy, HH:mm:ss");

            for(Pins pins : pinsList) {
                System.out.println(pins.getId().getDate());

                pins.setDate(outputFormat.format(pins.getId().getDate()));

            }
        return pinsList;
    }

    public Optional<Pins> findPinsById(String sheepId) {
        return pinsRepo.findPinsBySheepId(sheepId);
    }


    public Pins addPins(Pins pins) {
        System.out.println("Service:"+pins);
        System.out.println("work");
        System.out.println(pins);
        System.out.println(pins.getObjectJSON());
        System.out.println(pins.getDeviceName());
        System.out.println(pins.getDevEUI());


        String jsonString = pins.getObjectJSON();
        JSONObject json = new JSONObject(jsonString);


        pins.setLongitude(json.getDouble("longitude"));
        pins.setLatitude(json.getDouble("latitude"));
        pins.setSheepId(json.getString("name"));
        pins.setAccelero_x(json.getDouble("accelero_x"));
        pins.setGenId(String.valueOf((allPins().toArray().length + 1)));
        if(pins.getAccelero_x() < 0){
            count++;
            System.out.println("Count" + count);
        }
        else if(pins.getAccelero_x() > 0 &&count > 1){
            Twilio.init(ACCOUNT_SID, AUTH_TOKEN);
            Message message = Message.creator(
                    new com.twilio.type.PhoneNumber("whatsapp:+353877178072"),
                    new com.twilio.type.PhoneNumber("whatsapp:+14155238886"),
                    "Your Sheep is back up").create();

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
                    "Your Sheep is upside down").create();

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
