let express = require('express');
let router = express.Router();

let Mongoose = require('mongoose').Mongoose;
let Schema = require('mongoose').Schema;

let oldMong = new Mongoose();
oldMong.connect('mongodb://127.0.0.1:27017/db');

let pinSchema = new Schema({
  sheepId: String,
  longitude: Number,
  latitude: Number ,
  time : { type : Date, default: Date.now }
}, 
{ timestamps: true },
{ collection: 'pins' });

let pins = oldMong.model('pins',pinSchema);
// Admin server page
router.get('/', async function (req, res, next) {
  res.render('index');
});

router.post('/createPins', async function (req, res, next) {
  let retVal = { response: "fail" }
  await pins.create(req.body,
    function (err, res) {
      if (!err) {
        retVal = { response: "success" }
      }
    }
  )
  res.json(retVal);
});

router.post('/readPin', async function (req, res, next) {
  let data;
  if (req.body.cmd == 'all') {
    data = await pins.find().lean()
  }
  else {
    data = await pins.find({ _id: req.body._id }).lean()
  }
  res.json({ pins: data });
})




module.exports = router;