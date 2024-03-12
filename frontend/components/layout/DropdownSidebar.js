import classes from "./DropdownSidebar.module.css";

import { useContext } from "react";
import GlobalContext from "../../pages/store/globalContext";

import { useRouter } from "next/router";
import React from "react";



import {
  Navbar,
  NavbarBrand,
  NavbarContent,
  NavbarItem,
  NavbarMenuToggle,
  NavbarMenu,
  NavbarMenuItem,
  Button,
  Dropdown,
} from "@nextui-org/react";

function DropdownSidebar() {
  const [isMenuOpen, setIsMenuOpen] = React.useState(false);
  const globalCtx = useContext(GlobalContext);

  function clicked(longitude, latitude) {
    globalCtx.updateGlobals({
      cmd: "updateMapCenter",
      newCenter: [longitude, latitude],
    });
    console.log(longitude, latitude);
  }

  const contents = [];
  console.log(globalCtx.theGlobalObject.pins);
  globalCtx.theGlobalObject.pins.forEach((element) => {
    contents.push({
      title: element.sheepId,
      date: element.date,
      longitude: element.longitude,
      latitude: element.latitude,
    });
  });

  let contentJsx = contents.map((item, index) => (
    <div
      className={classes.menuItem}
      key={index}
      onClick={() => clicked(item.longitude, item.latitude)}
    >
      {item.title} {item.date}{" "}
    </div>
  ));
  return (
    <header className={classes.header}>
      <div>
        <Navbar style={{"--navbar-height":"6rem"} }className="w-screen justify-between backdrop-saturate-100 bg-transparent">
          <NavbarContent >
            <NavbarMenuToggle className="bg-transparent" />
          </NavbarContent>

          <NavbarContent>
            <NavbarBrand style={{ position:"fixed", left:"43vw"}} className='hidden sm:flex gap-4" justify-end'>
              <p className="font-bold text-inherit text-4xl">LoRaWAN Lamb Link</p>
            </NavbarBrand>
          </NavbarContent>
       
          <NavbarMenu style={{"--navbar-height":"6rem"} } className="max-w-sm backdrop-blur-2xl  bg-background overflow-y-scroll no-scrollbar ">
            <NavbarMenuItem>{contentJsx}</NavbarMenuItem>
          </NavbarMenu>
        </Navbar>
    
      </div>
      
    </header>
  );
}

export default DropdownSidebar;
