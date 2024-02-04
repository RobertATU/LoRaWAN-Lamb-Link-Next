package ie.atu.springbackend;

import org.springframework.beans.factory.annotation.Autowired;
import org.springframework.stereotype.Service;
import org.springframework.transaction.annotation.Transactional;

import java.util.List;
import java.util.Optional;

@Service
public class PinsService {
    @Autowired
    private PinsRepo pinsRepo;
    public List<Pins>allPins(){
        System.out.println(pinsRepo.findAll());
        return pinsRepo.findAll();
    }

    public Optional<Pins> findPinsById(String sheepId) {
        return pinsRepo.findPinsBySheepId(sheepId);
    }


    public Pins addPins(Pins pins) {
        return pinsRepo.save(pins);
    }

    @Transactional
    public Pins deletePin(String sheepId) {
        return pinsRepo.deleteBySheepId(sheepId);
    }
}
