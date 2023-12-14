package ie.atu.springbackend;

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
                System.out.println(pins);
        System.out.println(pinsService.addPins(pins).toString());
        return pinsService.addPins(pins);
    }

    @GetMapping("/{sheepId}")
    public ResponseEntity<Optional<Pins>> getSinglePin(@PathVariable String sheepId){
        return new ResponseEntity<Optional<Pins>>(pinsService.findPinsById( sheepId), HttpStatus.OK);
    }
}
