package ie.atu.springbackend;

import org.json.JSONObject;
import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.http.HttpStatus;
import org.springframework.http.ResponseEntity;
import org.springframework.web.bind.annotation.*;
import java.util.List;
import java.util.Optional;


@RestController
@RequestMapping("/api/pins")
public class PinsController {
    @Autowired
            private PinsService pinsService;
            @GetMapping
            public List<Pins> allPins(){

                return pinsService.allPins();
    }
    @PostMapping("/addPin")
    public Pins addPins(@RequestBody Pins pins){
                System.out.println("work");
        System.out.println(pins);
        System.out.println(pins.getObjectJSON());

        String jsonString = pins.getObjectJSON();
        JSONObject  json = new JSONObject(jsonString);
        pins.setLongitude(json.getDouble("longitude"));
        pins.setLatitude(json.getDouble("latitude"));
        pins.setSheepId(json.getString("name"));
        System.out.println(pins);

        return pinsService.addPins(pins);
    }

    @DeleteMapping("/removePin/{sheepId}")
    public Pins addPins(@PathVariable String sheepId){
        System.out.println(pinsService.deletePin(sheepId));
        return pinsService.deletePin(sheepId);
    }

    @GetMapping("/{sheepId}")
    public ResponseEntity<Optional<Pins>> getSinglePin(@PathVariable String sheepId){
        return new ResponseEntity<Optional<Pins>>(pinsService.findPinsById( sheepId), HttpStatus.OK);
    }
}
