package ie.atu.springbackend;
import lombok.AllArgsConstructor;
import lombok.Data;
import lombok.Generated;
import lombok.NoArgsConstructor;
import org.bson.types.ObjectId;
import org.springframework.data.annotation.Id;
import org.springframework.data.mongodb.core.mapping.Document;
@Data
@AllArgsConstructor
@NoArgsConstructor
@Document(collection = "pins")
public class Pins {
    @Id
    @Generated
    private ObjectId id;

    private String genId;

    private String sheepId;

    private double longitude;

    private double latitude;

    private double accelero_x;

    private String date;

    private String objectJSON;
}
