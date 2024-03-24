async function handler(req, res) { 
    try{console.log("\n\r" + req.body +"\n\r")
    const genId  = req.body.stringify;
    const response = await fetch('http://34.197.171.218/api/pins/removePin/' + req.body, {
      method: 'DELETE',
      headers: {
        'Content-Type': 'application/json'
      }
    });

    if (!response.ok){
      throw new Error('Failed to delete pin');
    }

    console.log('Pin deleted successfully');
    res.status(200).json({message: 'Pin deleted successfully'});
  }catch(error){
    console.error('Error deleting pin');
    res.status(500).json({message: 'Internal Server Error'});

  }
   
  }
  
  export default handler;