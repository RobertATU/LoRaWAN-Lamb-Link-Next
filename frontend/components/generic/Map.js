import { React, useEffect, useState } from "react";
import mapboxgl from "mapbox-gl";
import "mapbox-gl/dist/mapbox-gl.css";
import { useContext } from "react";
import GlobalContext from "../../pages/store/globalContext";
import { confirmAlert } from "react-confirm-alert";
import "react-confirm-alert/src/react-confirm-alert.css";
import { Card, CardBody } from "@nextui-org/react";
import { useRouter } from "next/router";
import classes from "./Map.module.css";

mapboxgl.accessToken =
  "pk.eyJ1Ijoicm9iZXJ0bXVsZG9vbmZ5cCIsImEiOiJjbHBlajVqejIxZDc5MmhybGZha3N3dGF3In0.EmdczNp0fNiA_es8l1-y9Q";
function Map() {
  const [map, setMap] = useState(null);
  const globalCtx = useContext(GlobalContext);
  const mapCenter = globalCtx.theGlobalObject.mapCenter;
  const zoom = globalCtx.theGlobalObject.zoom;
  const pins = globalCtx.theGlobalObject.pins;
  const router = useRouter();

  useEffect(() => {
    const timerId = setInterval(() => {
      console.log("Location: " + globalCtx.theGlobalObject.mapCenter);
      globalCtx.updateAll(
        globalCtx.theGlobalObject.mapCenter,
        globalCtx.theGlobalObject.zoom
      );
    }, 20000);
  }, []);

  useEffect(() => {
    console.log(globalCtx.theGlobalObject);
    console.log(zoom);
    console.log(pins);
    const initializeMap = () => {
      const mapInst = new mapboxgl.Map({
        container: "map",
        style: "mapbox://styles/mapbox/streets-v12",
        center: mapCenter,
        zoom: zoom,
      });
      mapInst.on("move", () => {
        globalCtx.theGlobalObject.mapCenter = mapInst.getCenter().toArray();
        globalCtx.theGlobalObject.zoom = mapInst.getZoom();
      });
      setMap(mapInst);
    };
    if (!map) {
      initializeMap();
    } else {
      map.setCenter(mapCenter);
      map.setZoom(zoom);
    }

    for (let i = 0; i < globalCtx.theGlobalObject.pins.length; i++) {
      var pin, status;

      if (i == globalCtx.theGlobalObject.pins.length - 1) {
        if (pins[i].accelero_x > 0) {
          status = "Ok";
        } else {
          status = "Needs assistance";
        }

        pin = new mapboxgl.Marker({ color: "#b40219" })
          .setLngLat([pins[i].longitude, pins[i].latitude])
          .addTo(map);
      } else {
        if (pins[i].accelero_x > 0) {
          status = "Ok";
        } else {
          status = "Needs assistance";
        }
        pin = new mapboxgl.Marker()
          .setLngLat([pins[i].longitude, pins[i].latitude])
          .addTo(map);
        console.log(pins[i].accelero_x);
      }

      const pinPopContent = document.createElement("div");
      pinPopContent.innerHTML = `<h3>${pins[i].sheepId}</h3><p>Seen at: ${pins[i].date}</p><p>Status: ${status}</p><button id="deleteButton_${i}">Delete</button>`;

      const pinPopup = new mapboxgl.Popup().setDOMContent(pinPopContent);
      pin.setPopup(pinPopup);

      pinPopContent
        .querySelector(`#deleteButton_${i}`)
        .addEventListener("click", () => {
          confirmAlert({
            title: "Confirm to Delete",
            message: "Are you want to delete this pin.",
            buttons: [
              {
                label: "Yes",
                onClick: () => deletePin(i),
              },
              {
                label: "No",
              },
            ],
          });
        });
      pin.getElement().addEventListener("click", () => {
        pin.togglePopup();
      });
    }

    const deletePin = async (i) => {
      await globalCtx.deletePins(pins[i].genId);
      router.reload();
    };
  }, [map, mapCenter]);

  return (
    <div>
      <Card className="py-5 px-4  bg-card backdrop-blur-2xl xl-card">
        <CardBody className="">
          <div id="map" className={classes.map}></div>
        </CardBody>
      </Card>
    </div>
  );
}

export default Map;
