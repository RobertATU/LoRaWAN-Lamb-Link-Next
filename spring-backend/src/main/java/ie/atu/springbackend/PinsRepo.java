package ie.atu.springbackend;

import org.bson.types.ObjectId;
import org.springframework.data.mongodb.repository.MongoRepository;
import org.springframework.stereotype.Repository;

import java.util.Optional;

@Repository
public interface PinsRepo extends MongoRepository<Pins, ObjectId> {
    Optional<Pins> findPinsBySheepId(String sheepId);

}
