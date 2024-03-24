import MainNavigation from './MainNavigation';
import DropdownSidebar from './DropdownSidebar';
import classes from './Layout.module.css';
import Image from 'next/image'
import {Card, CardHeader, CardBody, CardFooter} from "@nextui-org/react";
import GlobalContext from "../../pages/store/globalContext";
import { useContext } from "react";

function Layout(props) {
  const globalCtx = useContext(GlobalContext)
  function clicked() {
   
    globalCtx.updateGlobals({
      cmd: "updateZoom",
     
    })
  }

  function clickedSheep() {
   var length = globalCtx.theGlobalObject.pins.length -1;
   globalCtx.updateGlobals({
    cmd: "updateMapCenter",
    newCenter: [globalCtx.theGlobalObject.pins[length].longitude,globalCtx.theGlobalObject.pins[length].latitude],
  });
  }
  return (
    <div>
      <DropdownSidebar />
      <div className={classes.icon} >
      <Card className="py-4 bg-card backdrop-blur-2xl " >
      <CardHeader className="pb-0 pt-2 px-4 flex-col items-centre">
        <h4 className="font-bold text-large">Latest Location</h4>
      </CardHeader>
      <CardBody className="overflow-visible py-2">
        <Image
          src="/images/sheep (2).png"
          width={200}
          height={200}
          onClick={() => clickedSheep()}
          alt ="Sheep Icon"
          title='Icon from: https://www.flaticon.com/free-icons/sheep Sheep icons created by Ains - Flaticon'
        />
      </CardBody>
    </Card>
    </div>
    <div className={classes.iconMag} >
      <Card className="py-4 bg-card backdrop-blur-2xl " >
      <CardHeader className="pb-0 pt-2 px-4 flex-col items-centre">
        <h4 className="font-bold text-large">Reset Map</h4>
      </CardHeader>
      <CardBody className="overflow-visible py-2">
        <Image
          src="/images/tools.png"
          width={200}
          height={200}
          onClick={() => clicked()}
          alt ="Sheep Icon"
          title='Icon from: https://www.flaticon.com/free-icons/magnifying-glass Magnifying glass icons created by IconMark'
        />
      </CardBody>
     
    </Card>
    </div>
      <main className={classes.main}>{props.children}</main>
   
    </div>
  );
}

export default Layout;
