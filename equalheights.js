(function($) {
	Drupal.behaviors.equalHeightsModule = {
	  attach: function (context, settings) {
	  var eqClass = Drupal.settings.equalHeightsModule.eqClass;
	  var overflow = Drupal.settings.equalHeightsModule.overflow;
	    $.each(eqClass, function(eqClass, height) {		
		  $(eqClass).equalHeights(height.minheight, height.maxheight).css('overflow', overflow)});	    
	  }
	}	
})(jQuery);