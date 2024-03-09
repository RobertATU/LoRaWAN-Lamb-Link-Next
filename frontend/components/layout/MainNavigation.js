import classes from './MainNavigation.module.css'
import Link from 'next/link'
import HamMenu from "../generic/HamMenu"
import HamMenuFAB from "../generic/HamMenuFAB"
import { useContext } from 'react'
import GlobalContext from "../../pages/store/globalContext"
import HamMenuContent from "./HamMenuContent"
import { useRouter } from 'next/router'
import React from "react";
import {Navbar, NavbarBrand, NavbarContent, NavbarItem, NavbarMenuToggle, NavbarMenu, NavbarMenuItem, Button} from "@nextui-org/react";


function MainNavigation() {
  const [isMenuOpen, setIsMenuOpen] = React.useState(false);
  const globalCtx = useContext(GlobalContext)
  const router = useRouter()
  const menuItems = [
    "Profile",
    "Dashboard",
    "Activity",
    "Analytics",
    "System",
    "Deployments",
    "My Settings",
    "Team Settings",
    "Help & Feedback",
    "Log Out",
  ];

  function toggleMenuHide() {
    globalCtx.updateGlobals({ cmd: 'hideHamMenu', newVal: false })
  }

  const contents = []
  console.log(globalCtx.theGlobalObject.pins);
  globalCtx.theGlobalObject.pins.forEach(element => {
  
    contents.push({title: element.sheepId,date: element.id.date,longitude: element.longitude,latitude: element.latitude })
  });

  let contentJsx = contents.map((item, index) => (  //  [{title: 'Meeting 1', webAddress: '/meet1'}, {title: 'Meeting 2', webAddress: '/meet2'}]
  <div className={classes.menuItem} key={index} onClick={() => clicked(item.longitude,item.latitude)} >{item.title} {item.date} </div>
))
  return (
    <header className={classes.header}>
      <Navbar className='bg-transparent backdrop-saturate-0'>
       <NavbarMenuToggle className='bg-transparent'
     //  aria-label={() => toggleMenuHide()}
       aria-label={isMenuOpen ? " menu" : "Open menu"}
        />
         <NavbarMenu className='max-w-sm'>
      
          <NavbarMenuItem>
              {contentJsx}
        
          </NavbarMenuItem>

      </NavbarMenu>
        </Navbar>
      <HamMenuContent contents={contents} />
      <HamMenu toggleMenuHide={() => toggleMenuHide()} />
      <div className={classes.logo}>LoRaWAN-Lamb-Link</div>
      <HamMenuFAB toggleMenuHide={() => toggleMenuHide()} />
      
    </header>
  );
}

export default MainNavigation
