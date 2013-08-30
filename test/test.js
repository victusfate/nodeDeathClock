var deathClock = require('../build/Release/deathClock.node');
var DeathClock = deathClock.DeathClock;
var deathClockLib = require('../lib/DeathClock')

console.log('activating 3 death clocks');
 
var aDeathClock = new DeathClock(1,'end of the world a','',10000);
var bDeathClock = new DeathClock(2,'end of the world b','',10000);
var cDeathClock = new DeathClock(3,'end of the world c','',10000);
sleep(11000);
console.log( 'should never get here' ); // end of world a triggers at 1 second
