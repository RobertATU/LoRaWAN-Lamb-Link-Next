package ie.atu.springbackend;
import org.bson.types.ObjectId;
import org.springframework.data.mongodb.repository.MongoRepository;
import org.springframework.stereotype.Repository;
@Repository
public interface PinsRepo extends MongoRepository<Pins, ObjectId> {
    Pins deletePinsByGenId(String genId);
}
