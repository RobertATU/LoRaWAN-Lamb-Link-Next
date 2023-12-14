import classes from './MainNavigation.module.css'
import Link from 'next/link'
import HamMenu from "../generic/HamMenu"
import HamMenuFAB from "../generic/HamMenuFAB"
import { useContext } from 'react'
import GlobalContext from "../../pages/store/globalContext"
import HamMenuContent from "./HamMenuContent"
import { useRouter } from 'next/router'

function MainNavigation() {
  const globalCtx = useContext(GlobalContext)
  const router = useRouter()

  function toggleMenuHide() {
    globalCtx.updateGlobals({ cmd: 'hideHamMenu', newVal: false })
  }

  const contents = []
  console.log(globalCtx.theGlobalObject.pins);
  globalCtx.theGlobalObject.pins.forEach(element => {
  
    contents.push({title: element.sheepId, longitude: element.longitude,latitude: element.latitude })
  });

  return (
    <header className={classes.header}>
       
      <HamMenuContent contents={contents} />
      <HamMenu toggleMenuHide={() => toggleMenuHide()} />
      <div className={classes.logo}>LoRaWAN-Lamb-Link</div>
      <HamMenuFAB toggleMenuHide={() => toggleMenuHide()} />
      
    </header>
  );
}

export default MainNavigation
