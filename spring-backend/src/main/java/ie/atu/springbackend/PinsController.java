package ie.atu.springbackend;

import org.json.JSONObject;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.http.HttpStatus;
import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.*;

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


@RestController
@RequestMapping("/api/pins")
public class PinsController {
    // Find your Account Sid and Token at twilio.com/console
    public static final String ACCOUNT_SID = "";
    public static final String AUTH_TOKEN = "";
    public int count;
    @Autowired
            private PinsService pinsService;
            @GetMapping
            public List<Pins> allPins(){
                System.out.println("Count:" + count);
                return pinsService.allPins();
    }
    @PostMapping("/addPin")
    public Pins addPins(@RequestBody Pins pins){
                System.out.println("work");
        System.out.println(pins);
        System.out.println(pins.getObjectJSON());
        System.out.println(pins.getDeviceName());
        System.out.println(pins.getDevEUI());

        System.out.println(pins.getId().getDate());
        SimpleDateFormat outputFormat = new SimpleDateFormat("dd/MM/yyyy, HH:mm:ss");
        pins.setDate(outputFormat.format(pins.getId().getDate()));


        String jsonString = pins.getObjectJSON();
        JSONObject  json = new JSONObject(jsonString);

        pins.setGenId(pins.getId().toHexString());
        pins.setLongitude(json.getDouble("longitude"));
        pins.setLatitude(json.getDouble("latitude"));
        pins.setSheepId(json.getString("name"));
        pins.setAccelero_x(json.getDouble("accelero_x"));
        if(pins.getAccelero_x() < 0){
            count++;
            System.out.println("Count" + count);
        }
        else {
            count = 0;
            System.out.println("Count" + count);
        }

        if(count > 5){
                Twilio.init(ACCOUNT_SID, AUTH_TOKEN);
                Message message = Message.creator(
                        new com.twilio.type.PhoneNumber("whatsapp:+353877178072"),
                        new com.twilio.type.PhoneNumber("whatsapp:+14155238886"),
                        "Your Sheep is upside down").create();

                System.out.println(message.getSid());

        }
        System.out.println(pins);


        return pinsService.addPins(pins);
    }


    @DeleteMapping("/removePin/{genId}")
    public Pins addPins(@PathVariable String genId){
        System.out.println(pinsService.deletePin("\n\n\n\nTest:"+genId));
        return pinsService.deletePin(genId);
    }

    @GetMapping("/{sheepId}")
    public ResponseEntity<Optional<Pins>> getSinglePin(@PathVariable String sheepId){
        return new ResponseEntity<Optional<Pins>>(pinsService.findPinsById( sheepId), HttpStatus.OK);
    }
}
