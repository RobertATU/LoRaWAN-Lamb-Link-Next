async function handler(req, res) { 
    console.log("\n\r" + req.body +"\n\r")
    const sheepId  = req.body.stringify;
    const response = await fetch('http://localhost:8080/api/pins/removePin/' + req.body, {
      method: 'DELETE',
      headers: {
        'Content-Type': 'application/json'
      }
    })
   
  }
  
  export default handler;