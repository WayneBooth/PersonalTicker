var _MS_PER_DAY = 1000 * 60 * 60 * 24;

function getCalendarEvents(result, calendar, maxItems, format) {
  Logger.log('Entered getFamilyCalendarEvents');
  var calEvents = [];
  
  //var cals = Calendar.CalendarList.list().items;
  //if (cals.length > 0) {
  //  for (i = 0; i < cals.length; i++) {
  //    Logger.log( '%s = %s', cals[i].id, cals[i].description );
  //  }
  //}
  
  var calendarId;
  if( calendar == 'countdown' ) {
    result.push( blockStart, 'Looking Forward To');
    calendarId = COUNTDOWN;
  }
  else {
    result.push( blockStart, 'Family Calendar Events');
    calendarId = FAMILY;
  }

  var fromDate = new Date();
  var toDate = new Date(Date.now() + (80 * _MS_PER_DAY));
  //Logger.log("From %s to %s", fromDate.toISOString(), toDate.toISOString() );
  var optionalArgs = {
    timeMin: fromDate.toISOString(),
    timeMax: toDate.toISOString(),
    showDeleted: false,
    singleEvents: true,
    maxResults: maxItems,
    orderBy: 'startTime'
  };
  var response = Calendar.Events.list(calendarId, optionalArgs);
  var events = response.items;
  if (events.length > 0) {
    for (i = 0; i < events.length; i++) {
      var event = events[i];
      var when = event.start.dateTime;
      if (!when) {
        when = event.start.date;
      }
      if( format == 'days' ) {
        when = DateDiffInDays( new Date(), new Date(when) ) + " days until";
      }
      else {
        when = Utilities.formatDate( new Date(when), "GMT", "dd/MMM/yyyy");
      }
      
      var future_note = "";
      if(  DateDiffInDays( new Date(event.start.date), new Date(event.end.date) ) > 1 // longer than 1 day
        && (new Date()).valueOf() > (new Date(event.start.date)).valueOf() ) { // Already started
        future_note = " - ends in " + DateDiffInDays( new Date(), new Date(event.end.date) ) + " days";
      }
      
      //Logger.log('Event found.');
      result.push( itemStart, when + " \"" + event.summary + "\"" + future_note );
    }
  } else {
    Logger.log('No upcoming events found.');
  }

}

function DateDiffInDays(a, b) 
{
  // Discard the time and time-zone information.
  var utc1 = Date.UTC(a.getFullYear(), a.getMonth(), a.getDate());
  var utc2 = Date.UTC(b.getFullYear(), b.getMonth(), b.getDate());
  return Math.floor((utc2 - utc1) / _MS_PER_DAY);
}
