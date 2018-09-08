function getDate(result) {
  Logger.log('Entered getDate');
  
  result.push( blockStart, 'Current Date');
  result.push( itemStart, (new Date()).toDateString() );
}

function getTime(result) {
  Logger.log('Entered getTime');
  
  result.push( blockStart, 'Time Data Collected');
  result.push( itemStart, (new Date()).toTimeString() );
}

function showTime(result) {
  Logger.log('Entered getTime');
  
  result.push( blockStart, 'Current Time');
  result.push( itemStart, '#t' ); // read current timme locally
}