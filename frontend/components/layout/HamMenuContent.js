import classes from './HamMenuContent.module.css'
import { useRouter } from 'next/router'
import { useContext, useState } from 'react'
import GlobalContext from "../../pages/store/globalContext"

export default function HamMenuContent(props) {
    const globalCtx = useContext(GlobalContext)
    const router = useRouter()
    let [popupToggle, setPopupToggle] = useState(false)

    if (globalCtx.theGlobalObject.hideHamMenu) {
        return null
    }

    function clicked(longitude,latitude) {
        globalCtx.updateGlobals({ cmd: 'updateMapCenter', newCenter: [longitude,latitude] })
        console.log(longitude,latitude)
    }

    function closeMe() {
        globalCtx.updateGlobals({ cmd: 'hideHamMenu', newVal: true })
        if (popupToggle == true) {
            setPopupToggle(false)
        } else {
            setPopupToggle(true)
        }
    }
console.log(props.contents)
    let contentJsx = props.contents.map((item, index) => (  //  [{title: 'Meeting 1', webAddress: '/meet1'}, {title: 'Meeting 2', webAddress: '/meet2'}]
        <div className={classes.menuItem} key={index} onClick={() => clicked(item.longitude,item.latitude)} >{item.title} </div>
    ))

    return (
        <div className={classes.background} onClick={() => closeMe()} >
            <div className={classes.mainContent} >
                {contentJsx}
            </div>
        </div>
    );
}
