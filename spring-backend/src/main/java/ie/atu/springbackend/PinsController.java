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
        return pinsService.addPins(pins);
    }


    @DeleteMapping("/removePin/{genId}")
    public Pins addPins(@PathVariable String genId){
        return pinsService.deletePin(genId);
    }

    @DeleteMapping("/removeAll")
    public void removeAllPins(){
         pinsService.deleteAllPin();
    }

    @GetMapping("/{sheepId}")
    public ResponseEntity<Optional<Pins>> getSinglePin(@PathVariable String sheepId){
        return new ResponseEntity<Optional<Pins>>(pinsService.findPinsById( sheepId), HttpStatus.OK);
    }
}
