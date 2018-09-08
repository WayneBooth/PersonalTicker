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
    calendarId = 'q68tiidhll1edg62lssbmc2lq8@group.calendar.google.com';
  }
  else {
    result.push( blockStart, 'Family Calendar Events');
    calendarId = 'family07783842730088701806@group.calendar.google.com';
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
      
      //Logger.log('Event found.');
      result.push( itemStart, when + " \"" + event.summary + "\"" );
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
