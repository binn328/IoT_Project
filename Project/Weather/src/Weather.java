

import org.json.simple.*;
import org.json.simple.parser.*;
import java.net.*;
import java.io.*;

import java.time.*;
import java.time.format.*;
import java.util.*;




public class Weather{
	
	
   public static void main(String[] args) throws IOException, ParseException {
     
	  File file = new File("./java_api.txt");
	  if (!file.exists()) {
		  file.createNewFile();
		  }

		 try (FileOutputStream fos = new FileOutputStream(file);
	             PrintWriter writer = new PrintWriter(fos)) {
	             
	            getWeatherInfo(writer);
	            getDustInfo(writer);
	            
	        } catch (IOException e) {
	            e.printStackTrace();
	        }
	    }
       
    
   
   
   public static void getWeatherInfo(PrintWriter writer) throws IOException, ParseException{
	   
	   
	   String apiUrl = "http://apis.data.go.kr/1360000/VilageFcstInfoService_2.0/getVilageFcst";
      
      String serviceKey = "fWRe92lUenF956iBOQ5mghAic9lMcAQ5wNdWfT6hb4aaBD6XmT6Lr9gc9%2FKJI3FfaL37G8IrPkwUgEZbNtloSA%3D%3D";
      String nx = "92";
      String ny = "90";
      
      LocalDateTime currentTime = LocalDateTime.now();
      
      DateTimeFormatter dateFormatter = DateTimeFormatter.ofPattern("yyyyMMdd");
      String baseDate = currentTime.format(dateFormatter);

   // 현재 시간 구하기
      Calendar cal = Calendar.getInstance();
      int hour = cal.get(Calendar.HOUR_OF_DAY);

      // 가져올 자료의 시간 계산
      String targetHour;
      if (hour < 2) {
          targetHour = "2300";
      } else if (hour <= 5) {
          targetHour = "0200";
      } else if (hour <= 8) {
          targetHour = "0500";
      } else if (hour <= 11) {
          targetHour = "0800" ;
      } else if (hour <= 14) {
          targetHour = "1100";
      } else if (hour <= 17) {
          targetHour = "1400";
      } else if (hour <= 20) {
          targetHour = "1700";
      } else {
          targetHour = "2000";
      }
      
      
      String strHour = Integer.toString(hour);  
      
      String baseTime = targetHour;   //API 제공 시간 , 2/5/8/11/14/17/20/23
      String type = "json";   //타입 xml, json 등등 ..
      String numOfRows = "253";   //한 페이지 결과 수 
      
        StringBuilder urlBuilder = new StringBuilder(apiUrl);
        urlBuilder.append("?" + URLEncoder.encode("ServiceKey","UTF-8") + "="+serviceKey);
        urlBuilder.append("&" + URLEncoder.encode("nx","UTF-8") + "=" + URLEncoder.encode(nx, "UTF-8"));
        urlBuilder.append("&" + URLEncoder.encode("ny","UTF-8") + "=" + URLEncoder.encode(ny, "UTF-8"));
        urlBuilder.append("&" + URLEncoder.encode("base_date","UTF-8") + "=" + URLEncoder.encode(baseDate, "UTF-8"));
        urlBuilder.append("&" + URLEncoder.encode("base_time","UTF-8") + "=" + URLEncoder.encode(baseTime, "UTF-8"));
        urlBuilder.append("&" + URLEncoder.encode("dataType","UTF-8") + "=" + URLEncoder.encode(type, "UTF-8"));
        urlBuilder.append("&" + URLEncoder.encode("numOfRows","UTF-8") + "=" + URLEncoder.encode(numOfRows, "UTF-8"));
        
        
        URL url = new URL(urlBuilder.toString());
        //어떻게 넘어가는지 확인하고 싶으면 아래 출력분 주석 해제
        //System.out.println(url);
        HttpURLConnection conn = (HttpURLConnection) url.openConnection();
        conn.setRequestMethod("GET");
        conn.setRequestProperty("Content-type", "application/json");
        //System.out.println("Response code: " + conn.getResponseCode());
       
        
        BufferedReader rd;
        if(conn.getResponseCode() >= 200 && conn.getResponseCode() <= 300) {
           rd = new BufferedReader(new InputStreamReader(conn.getInputStream()));
        }
        else {
           rd = new BufferedReader(new InputStreamReader(conn.getErrorStream()));
        }
        
        StringBuilder sb = new StringBuilder();
        String line;
        
        while ((line = rd.readLine()) != null) {
           sb.append(line);
        }
        
        rd.close();
        conn.disconnect();
        String result= sb.toString();
        //System.out.println(result);
        
        
        // Json parser를 만들어 만들어진 문자열 데이터를 객체화 
        JSONParser parser = new JSONParser(); 
        JSONObject obj = (JSONObject) parser.parse(result); 
        // response 키를 가지고 데이터를 파싱 
        JSONObject parse_response = (JSONObject) obj.get("response"); 
        // response 로 부터 body 찾기
        JSONObject parse_body = (JSONObject) parse_response.get("body"); 
        // body 로 부터 items 찾기 
        JSONObject parse_items = (JSONObject) parse_body.get("items");

        // items로 부터 itemlist 를 받기 
        JSONArray parse_item = (JSONArray) parse_items.get("item");
        String category;
        JSONObject weather; // parse_item은 배열형태이기 때문에 하나씩 데이터를 하나씩 가져올때 사용
        // 카테고리와 값만 받아오기
        String day="";
        String time="";
        
        boolean hasRain = false;
      
        String latestDate=null;
        String latestTime=null;
        
         
        for(int i = 0 ; i < parse_item.size(); i++) {  //현재 api 업데이트 시간으로 해당 시간만 가져 온 시간. TMP(현재 기온)안뜸
        	weather = (JSONObject) parse_item.get(i);
        	Object fcstValue = weather.get("fcstValue");
        	Object fcstDate = weather.get("fcstDate");
        	Object fcstTime = weather.get("fcstTime");

        	if(fcstValue != null && fcstDate != null && fcstTime != null) {
  
        		category = (String)weather.get("category");

        	    if(category!=null && (category.startsWith("TMP") ||category.startsWith("POP")
        	        ||category.startsWith("SKY")|| category.startsWith("PTY"))) {

        	    	if(latestDate == null && latestTime == null) { 
        	    		latestDate = fcstDate.toString();
        	            latestTime = fcstTime.toString();
        	            day = fcstDate.toString();
        	            time = fcstTime.toString();
        	            
        	            }

        	    		if(strHour.equals(fcstTime.toString().substring(0, 2))) {        	                
        	                
        	    			if(category.equals("SKY") && fcstValue.equals("1")) {
        	    				//sunny
        	    				writer.write("0 ");
        	    			}else if(category.equals("SKY") 
        	    					&& (fcstValue.equals("3") || fcstValue.equals("4"))){
        	    				 //cloudy
        	    				writer.write("1 ");
        	    			}
        	    			else if(category.equals("PTY") && !fcstValue.equals("0")) {
        	    				//rain
        	    				writer.write("2 ");
        	    			}
        	    		}
        	    		
        	    	}
        	    
        	    if (category.equals("PTY") && !fcstValue.equals("0")) {
        	    	hasRain = true; //PTY:강수형태  0 = 없음:비 안옴 1= 비 2 = 비/눈 3 =눈 4= 소나기
        	    	}
        	   }
        }
        	

        if (hasRain) {
        	
        	writer.write("1 ");//비옴
        }
        
        else {
        	
        	writer.write("0 ");//비안옴
        }
   }

               
            	  
           	  
        	    
        
 static void getDustInfo(PrintWriter writer) throws IOException, ParseException{
      String serviceKey = "fWRe92lUenF956iBOQ5mghAic9lMcAQ5wNdWfT6hb4aaBD6XmT6Lr9gc9/KJI3FfaL37G8IrPkwUgEZbNtloSA==";
        
        StringBuilder urlBuilder = new StringBuilder("https://apis.data.go.kr/B552584/ArpltnStatsSvc/getCtprvnMesureSidoLIst"); /*URL*/
        urlBuilder.append("?serviceKey=").append(URLEncoder.encode(serviceKey, "UTF-8"));
        urlBuilder.append("&returnType=").append(URLEncoder.encode("json", "UTF-8"));
        urlBuilder.append("&numOfRows=").append(URLEncoder.encode("100", "UTF-8"));
        urlBuilder.append("&pageNo=").append(URLEncoder.encode("1", "UTF-8"));
        
        urlBuilder.append("&sidoName=").append(URLEncoder.encode("경북", "UTF-8"));
        urlBuilder.append("&searchCondition=").append(URLEncoder.encode("DAILY", "UTF-8"));
        

            URL url = new URL(urlBuilder.toString());
            HttpURLConnection conn = (HttpURLConnection) url.openConnection();
            conn.setRequestMethod("GET");
            conn.setRequestProperty("Content-type", "application/json");

            
            BufferedReader rd;
            if(conn.getResponseCode() >= 200 && conn.getResponseCode() <= 300) {
               rd = new BufferedReader(new InputStreamReader(conn.getInputStream()));
            } 
            else {
               rd = new BufferedReader(new InputStreamReader(conn.getErrorStream()));
            }

            StringBuilder sb = new StringBuilder();
            String line;
            while ((line = rd.readLine()) != null) {
               sb.append(line);
            }
            
            rd.close();
            conn.disconnect();
            String result= sb.toString();
            
            
            JSONParser parser = new JSONParser();
            JSONObject jsonObj = (JSONObject) parser.parse(result); 
            

            JSONObject responseObj = (JSONObject) jsonObj.get("response");
            JSONObject bodyObj = (JSONObject) responseObj.get("body");
            JSONArray itemsArr = (JSONArray) bodyObj.get("items");

            


               	
               JSONObject itemObj = (JSONObject) itemsArr.get(0);
               String cityName = (String) itemObj.get("cityName");

               if(cityName.equals("경산시")) {
                   String dataTime = (String) itemObj.get("dataTime");
                   String pm10Value = (String) itemObj.get("pm10Value");

                   
                   int pm10ValueInt = Integer.parseInt(pm10Value);

                   if(pm10ValueInt >= 0 && pm10ValueInt <= 30) {
                       //좋음
                	   writer.write("0 ");
                   } else if(pm10ValueInt >= 31 && pm10ValueInt <= 80) {
                       //보통
                	   writer.write("1 ");
                   } else {
                        //나쁨
                	   writer.write("2 ");
                   }
              }
   
   }
}