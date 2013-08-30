var deathClock = require('../build/Release/deathClock.node');
var DeathClock = deathClock.DeathClock;
var deathClockLib = require('../lib/DeathClock')

console.log('activating 3 death clocks');
 
var aDeathClock = new DeathClock(1,'end of the world a',10000);
var bDeathClock = new DeathClock(2,'end of the world b',10000);
var cDeathClock = new DeathClock(3,'end of the world c',10000);

aDeathClock.Stop();
bDeathClock.Stop();

function sleep(millis)
{
 	var date = new Date();
  	var curDate = null;
  	do { curDate = new Date(); }
  	while(curDate-date < millis);
}

sleep(11000);

console.log( 'should never get here' ); // end of world c triggers at 3 seconds
