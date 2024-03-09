import Maps from '../components/generic/Map'
import { useContext } from "react";
import GlobalContext from "./store/globalContext"




function HomePage() {
    const globalCtx = useContext(GlobalContext)


    if (globalCtx.theGlobalObject.dataLoaded == true) {
        return <Maps pins={globalCtx.theGlobalObject.pins}/>
    }
    return <div>Loading data from database, please wait . . . </div>
}

export default HomePage;