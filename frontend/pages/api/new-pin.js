
async function handler(req, res) { 
  const response = await fetch('http://34.197.171.218:8080/api/pins/addPin', {
    method: 'POST',
    body: JSON.stringify(req.body),
    headers: {
      'Content-Type': 'application/json'
    }
  });
  const data = await response.json();
  res.json(data)
}

export default handler;
