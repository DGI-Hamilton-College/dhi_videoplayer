// PLUGIN: WIKIPEDIA


var wikiCallback;

(function (Popcorn) {
  
  /**
   * Wikipedia popcorn plug-in 
   * Displays a wikipedia aricle in the target specified by the user by using
   * new DOM element instead overwriting them
   * Options parameter will need a start, end, target, lang, src, title and numberofwords.
   * -Start is the time that you want this plug-in to execute
   * -End is the time that you want this plug-in to stop executing 
   * -Target is the id of the document element that the text from the article needs to be  
   * attached to, this target element must exist on the DOM
   * -Lang (optional, defaults to english) is the language in which the article is in.
   * -Src is the url of the article 
   * -Title (optional) is the title of the article
   * -numberofwords (optional, defaults to 200) is  the number of words you want displaid.  
   *
   * @param {Object} options
   * 
   * Example:
     var p = Popcorn('#video')
        .wikipedia({
          start: 5, // seconds
          end: 15, // seconds
          src: 'http://en.wikipedia.org/wiki/Cape_Town',
          target: 'wikidiv'
        } )
   *
   */
  Popcorn.plugin( "wikipedia" , {
      
    manifest: {
      about:{
        name: "Popcorn Wikipedia Plugin",
        version: "0.1",
        author: "@annasob",
        website: "annasob.wordpress.com"
      },
      options:{
        start         : {elem:'input', type:'text', label:'In'},
        end           : {elem:'input', type:'text', label:'Out'},
        lang          : {elem:'input', type:'text', label:'Language'},
        src           : {elem:'input', type:'text', label:'Src'},
        title         : {elem:'input', type:'text', label:'Title'},
        numberofwords : {elem:'input', type:'text', label:'Num Of Words'},
        target        : 'wiki-container'
      }
    },
    /**
     * @member wikipedia 
     * The setup function will get all of the needed 
     * items in place before the start function is called. 
     * This includes getting data from wikipedia, if the data
     * is not received and processed before start is called start 
     * will not do anything
     */
    _setup : function( options ) {
      // declare needed variables
      // get a guid to use for the global wikicallback function
      var  _text, _guid = Popcorn.guid(); 
      
      // if the user didn't specify a language default to english
      if (typeof options.lang === 'undefined') { options.lang ="en"; }
      // if the user didn't specify number of words to use default to 200 
      options.numberofwords  = options.numberofwords || 250;
            
      // wiki global callback function with a unique id
      // function gets the needed information from wikipedia
      // and stores it by appending values to the options object
      window["wikiCallback"+ _guid]  = function (data) { 
        options._link = document.createElement('a');
        options._link.setAttribute('href', options.src);
        options._link.setAttribute('target', '_blank');
        // add the title of the article to the link
        options._link.innerHTML = data.parse.displaytitle;
        // get the content of the wiki article
        options._desc = document.createElement('p');
        // get the article text and remove any special characters
        _text = data.parse.text["*"].substr(data.parse.text["*"].indexOf('<p>'));
        _text = _text.replace(/((<(.|\n)+?>)|(\((.*?)\) )|(\[(.*?)\]))/g, "");
        options._desc.innerHTML = _text.substr(0,  options.numberofwords ) + " ...";
        
        options._fired = true;
      };
      
      var head   = document.getElementsByTagName("head")[0];
      var script = document.createElement("script");
      script.src = "http://"+options.lang+".wikipedia.org/w/api.php?action=parse&props=text&page=" + ( options.title || options.src.slice(options.src.lastIndexOf("/")+1)) + "&format=json&callback=wikiCallback"+ _guid;

      head.insertBefore( script, head.firstChild );        
    },
    /**
     * @member wikipedia 
     * The start function will be executed when the currentTime 
     * of the video  reaches the start time provided by the 
     * options variable
     */
    start: function(event, options){
      // dont do anything if the information didn't come back from wiki
      var isReady = function () {
        
        if ( !options._fired ) {
          setTimeout(function () {
            isReady();
          }, 13);
        } else {
      
          if (options._link && options._desc) {
            if ( document.getElementById( options.target ) ) {
              document.getElementById( options.target ).appendChild(options._link);
              document.getElementById( options.target ).appendChild(options._desc);
              options._added = true;
            }
          }
        }
      };
      
      isReady();
    },
    /**
     * @member wikipedia 
     * The end function will be executed when the currentTime 
     * of the video  reaches the end time provided by the 
     * options variable
     */
    end: function(event, options){
      // ensure that the data was actually added to the 
      // DOM before removal
      if (options._added) {
        document.getElementById( options.target ).removeChild(options._link);
        document.getElementById( options.target ).removeChild(options._desc);
      }
    }
     
  });

})( Popcorn );
