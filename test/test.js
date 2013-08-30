var deathClock = require('../build/Release/deathClock.node');
var DeathClock = deathClock.DeathClock;

console.log('activating 3 death clocks');
 
var aDeathClock = new DeathClock(1.1,'end of the world a');
var bDeathClock = new DeathClock(2,'end of the world b');
var cDeathClock = new DeathClock(1,'end of the world c');

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

console.log( 'should never get here' ); // end of world c triggers at 1 seconds
