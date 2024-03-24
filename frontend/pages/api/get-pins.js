async function handler(req, res) { 
  const response = await fetch('http://34.197.171.218:8080/api/pins', {
    method: 'GET',
    headers: {
      'Content-Type': 'application/json'
    }
  })
  const data = await response.json();
  console.log(JSON.stringify(data))
  res.json(data)
}

export default handler;
