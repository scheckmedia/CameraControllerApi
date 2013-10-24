window.Camera = Backbone.Model.extend({

	rootUrl : window.location.protocol + '//' + window.location.hostname + (location.port && ":" + location.port) + "/",
	settings : null,	
	images : null,
	cameraFound : false,

    initialize:function () {            	
        this.connect();        
    },

    getFileList : function(callback){
    	if(this.images == null){
    		Backbone.Notifications.trigger("loader:show");
	    	var $this = this;
	    	this.sendRequest('fs?action=list', function(result){    		    		
	    		Backbone.Notifications.trigger("loader:hide");
	    		if($this.isSuccess(result)){
	    			if(callback)
	    				callback(result.cca_response.data.folder);

	    			this.images = result;
	    		}    		    		
	    		Backbone.Notifications.trigger("alert", 1, "status: " + result.cca_response.state, 3000);
	    	});	
    	} else {
    		if(callback)
    			callback(this.images);
    	}   
    },

    reloadFileList : function(){
    	this.images = null;
    	getFileList();
    },

    getImage : function(image, path, callback){
		Backbone.Notifications.trigger("loader:show");
		var $this = this;
    	this.sendRequest('fs?action=get&value='+image+'&path='+path, function(result){    		    		
    		Backbone.Notifications.trigger("loader:hide");
    		if($this.isSuccess(result)){
    			if(callback)
    				callback(result.cca_response.data);

    			this.images = result;
    		}    		    		
    		Backbone.Notifications.trigger("alert", 1, "status: " + result.cca_response.state, 3000);
    	});

    },

    shot : function(){
    	Backbone.Notifications.trigger("loader:show");
    	var $this = this;
    	this.sendRequest('capture?action=shot', function(result){    		    		
    		Backbone.Notifications.trigger("loader:hide");
    		if($this.isSuccess(result)){
    			$('#result_image').attr("src", "data:image/gif;base64," + result.cca_response.data.image);	
    		}    		    		
    		Backbone.Notifications.trigger("alert", 1, "status: " + result.cca_response.state, 3000);
    	});
    },

    connect : function() {
    	Backbone.Notifications.trigger("loader:show");
    	var $this = this;
    	this.sendRequest('camera?action=status', function(result){    		    
    		if($this.isSuccess(result))		{
    			$this.cameraFound = true;
    			$this.sendRequest('settings?action=list', function(result){    		    		
    				if($this.isSuccess(result)){
    					$this.settings = result.cca_response.data.main;
    					Backbone.Notifications.trigger("alert", 1, "connection " + result.cca_response.state, 3000);
    				} else {
    					Backbone.Notifications.trigger("alert", 4, "connection " + result.cca_response.state, 3000);
    				}	    			
	    			Backbone.Notifications.trigger("loader:hide");
	    		});    		
    		}    		
    	});
    },

    setKeyValue : function(key, value){
    	Backbone.Notifications.trigger("loader:show");
    	var $this = this;
    	this.sendRequest('settings?action='+key+'&value='+value, function(result){    		    
    		if($this.isSuccess(result)){
				Backbone.Notifications.trigger("alert", 1, "connection " + result.cca_response.state, 3000);
			} else {
				Backbone.Notifications.trigger("alert", 4, "connection " + result.cca_response.state, 3000);
			}	    			
			Backbone.Notifications.trigger("loader:hide");
    	});
    },

    sendRequest : function(params, callback){    	
    	$.ajax({
            url: this.rootUrl + params,
            //url: "http://localhost:8888/" + params,
            dataType : "json",            
            success:function(result){            	
                callback(result);
            }, 
            error : function(xhr, status, error){            	
            	Backbone.Notifications.trigger("loader:hide");
            	Backbone.Notifications.trigger("alert", 4, "status " + status, 3000);
            }
        });
    },

    isSuccess : function(result){
    	if(result.cca_response.state == "success")return true;
    	else false;
    }
});

window.Navigation = Backbone.View.extend({
	el : '#navi',

	initialize : function(){
		this.render();
	},

	render : function(){
		var template = _.template( $("#navigation_template").html(), {} );
		this.$el.html( template );
	},

	events : {
		'click a' : 'navigate',		
	},

	navigate: function(el) {
		$(this.el).find('.active').removeClass('active');
        Backbone.history.navigate($(el.target).attr('href'), { trigger: true });
        $(el.target).parent().addClass('active');
        return false;
    }
});

window.Loader = Backbone.View.extend({
	el : '#loader',

	initialize : function(){
		Backbone.Notifications.on("loader:hide", this.hide, this);
		Backbone.Notifications.on("loader:show", this.show, this);
	},

	hide : function(){
		$(this.el).fadeOut();
	},

	show : function(){
		$(this.el).fadeIn();
	}

});

window.StatusBox = Backbone.View.extend({
	el : '#status',
	
	initialize : function(){
		Backbone.Notifications.on("alert", this.setAlert, this);
	},

	setAlert: function(status, text, time) {		
		var alertClass = "";
		switch(status){
			case 1:
				alertClass = "alert-success";
				break;

			case 2:
				alertClass = "alert-info";
				break;

			case 3:
				alertClass = "alert-warning";
				break;

			case 4:
				alertClass = "alert-danger";
				break;

			default:
				alertClass = "";
		}

		$(this.el).stop().fadeIn().removeAttr("class").addClass("alert " + alertClass).html(text).delay(time).fadeOut();
    }
});

window.EditBox = Backbone.View.extend({
	el : '#dialog',
	caman : null,

	initialize : function(){
		
	},

	events : {
		'click .exit' : 'close',		
		'click .save' : 'save',
		'slide .slider' : 'slide',
		'slideStop .slider' : 'slideStop'
	},

	open : function(base64){		
		var template = _.template( $("#editbox_template").html(), {} );			
		this.$el.html( template );

		$('#editbox').modal('show');		
		this.$el.find('.image-container').html('<img id="camanimage" src="data:image/jpg;base64,'+base64+'" data-camanwidth="800" data-camanheight="600"/>');		
		$('.slider').slider()		

		this.caman = Caman("#camanimage", function(){
			this.resize({
				width  : 800,
				height : 600
			});
			this.render();
		});
	},	

	save : function(){
		console.log("save");
	},

	close : function(){
		$('#editbox').modal('hide');
		this.caman = null;
	},

	slide : function(e){
		if(this.caman == null) return;
		console.log(e.value);
		var mode = $(e.target).attr('data-mode');		
		if(mode == "brightness")
			this.caman.brightness(e.value).render();
		else if(mode == "saturation")
			this.caman.saturation(e.value).render();
		else if(mode == "exposure")
			this.caman.exposure(e.value).render();
		else if(mode == "contrast")
			this.caman.contrast(e.value).render();
		else if(mode == "vibrance")
			this.caman.vibrance(e.value).render();
		else if(mode == "hue")
			this.caman.hue(e.value).render();
	},

	slideStop : function(){				
	}
});

window.ControlView = Backbone.View.extend({
	isConfigured : false,
	model : Camera,

	initialize : function(){
		this.render();
	},

	render : function(){
		var template = _.template( $("#control_template").html(), {} );
		this.$el.html( template );		
		$('btn').tooltip();
	},

	events : {
		'click button#shot' : 'shot',
		'click button#show' : 'fullscreen',
		'click button#settings' : 'settings',
		'click button#show-control' : 'fullscreen',
		'change #aperture' : 'change',
		'change #shutterspeed' : 'change',
		'change #iso' : 'change',
		'change #whitebalance' : 'change'
	},

	shot : function(){
		app.camera.shot();
	},

	fullscreen : function(){
		$('#controls').slideToggle(function(){
			$('#show-control').slideToggle('slow', 'easeOutBounce');
			$('#settings-panel').slideUp('slow', 'easeOutBounce');			
		});
	},

	settings : function(){
		if(this.isConfigured == false){
			this.configure();
		}
		$('#settings-panel').slideToggle('slow', 'easeOutBounce');
	},

	change : function(e){		
		var el = $(e.target);
		app.camera.setKeyValue(el.attr("name"), el.val());
	},

	configure : function(){
		if(!app.camera.cameraFound)
			return;

		var oIso = app.camera.settings.imgsettings.iso;
		var el = $('#iso');
		this.setOptionForElement(el, oIso.choices, oIso.value);

		var oAperture = app.camera.settings.capturesettings["f-number"];
		var el = $('#aperture');
		this.setOptionForElement(el, oAperture.choices, oAperture.value);

		var oShutterspeed = app.camera.settings.capturesettings.shutterspeed2;
		var el = $('#shutterspeed');
		this.setOptionForElement(el, oShutterspeed.choices, oShutterspeed.value);

		var oWhiteBalance = app.camera.settings.imgsettings.whitebalance;
		var el = $('#whitebalance');
		this.setOptionForElement(el, oWhiteBalance.choices, oWhiteBalance.value);	

		this.isConfigured = true;	
	},

	setOptionForElement : function(el, values, selected){
		$.each(values, function(index, value){			
			var option = $('<option></option>').val(value).html(value);
			if(selected == value)
				option.attr("selected","selected");

			el.append(option);
		});
	}
});

window.GalleryView = Backbone.View.extend({
	initialize : function(){
		this.render();		
		this.loadItems();	
	},

	render : function(){
		var template = _.template( $("#gallery_template").html(), {} );
		this.$el.html( template );				
	},

	loadItems : function(){
		app.camera.getFileList(function(response){
			var container = $('#images');
			$.each(response.files, function(index, value){				
				if(value.thumbnail){
					var img = '<img src="data:image/jpg;base64,'+value.thumbnail+'" alt="'+value.name+'" class="img-thumbnail" data-path="'+response.absolute_path+'" data-name="'+value.name+'">';
					container.append(img);
				}
			});
		});
	},

	events : {
		'click img' : 'openImage',		
	},

	openImage : function(e){
		var path = $(e.target).attr('data-path');
		var image = $(e.target).attr('data-name');

		app.camera.getImage($.trim(image),$.trim(path),function(result){			
			var editbox = new EditBox();
			editbox.open(result.image);
		});
	},
});

window.InfoView = Backbone.View.extend({
	initialize : function(){
		this.render();
	},

	render : function(){
		var template = _.template( $("#info_template").html(), {} );
		this.$el.html( template );				
	},
});


window.CameraControllerApi = Backbone.Router.extend({
	routes : {
		"" 			: "control",
		"control" 	: "control",
		"gallery" 	: "gallery",
		"info"		: "info"
	},

	initialize : function(){		
		this.camera = new Camera();
		this.statusBox = new StatusBox();	
		this.loader = new Loader();	
	},

	control : function(){
		if(!this.vControl){
			this.vControl = new ControlView();			
		} else {
			this.vControl.delegateEvents();
		}
		$('#content').html(this.vControl.el).removeAttr("class").addClass("control");		
	},

	gallery : function(){
		if(!this.vGallery){
			this.vGallery = new GalleryView();
			this.vGallery.render();
		} else {
			this.vGallery.delegateEvents();
		}

		$('#content').html(this.vGallery.el).removeAttr("class").addClass("gallery");	
	},

	info : function(){
		if(!this.vInfo){
			this.vInfo = new InfoView();
			this.vInfo.render();
		} else {
			this.vInfo.delegateEvents();
		}

		$('#content').html(this.vInfo.el).removeAttr("class").addClass("gallery");	
	}
	
});

window.utils = {

    // Asynchronously load templates located in separate .html files
    loadTemplate: function(views, callback) {

        var deferreds = [];

        $.each(views, function(index, view) {
            if (window[view]) {
                deferreds.push($.get('tpl/' + view + '.html', function(data) {
                    window[view].prototype.template = _.template(data);
                }));
            } else {
                alert(view + " not found");
            }
        });

        $.when.apply(null, deferreds).done(callback);
    },
};

$(function() {
	Backbone.Notifications = {};
	_.extend(Backbone.Notifications, Backbone.Events);

	nav = new Navigation();
    app = new CameraControllerApi();
	Backbone.history.start();
});
