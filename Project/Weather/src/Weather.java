import java.util.*;
import java.io.InputStreamReader;
import java.net.HttpURLConnection;
import java.net.URL;
import java.net.URLEncoder;
import java.io.BufferedReader;
import java.io.IOException;

import java.time.LocalDateTime;
import java.time.format.DateTimeFormatter;

import org.json.simple.JSONArray;
import org.json.simple.JSONObject;
import org.json.simple.parser.JSONParser;
import org.json.simple.parser.ParseException;


public class Weather {
   public static void main(String[] args) throws IOException, ParseException {
      
      Date date = new Date();
      
      
      String apiUrl = "http://apis.data.go.kr/1360000/VilageFcstInfoService_2.0/getVilageFcst";
      // 홈페이지에서 받은 키
 
      String serviceKey = "fWRe92lUenF956iBOQ5mghAic9lMcAQ5wNdWfT6hb4aaBD6XmT6Lr9gc9%2FKJI3FfaL37G8IrPkwUgEZbNtloSA%3D%3D";
      String nx = "92";   //위도
      String ny = "90";   //경도
      
   // 현재 시간 구하기
      LocalDateTime currentTime = LocalDateTime.now();
      
      DateTimeFormatter dateFormatter = DateTimeFormatter.ofPattern("yyyyMMdd");
      String baseDate = currentTime.format(dateFormatter);

      String baseTime = "0200";   //API 제공 시간 , 2/5/8/11/14/17/20/23
      String type = "json";   //타입 xml, json 등등 ..
      String numOfRows = "253";   //한 페이지 결과 수 
      
      
     
      StringBuilder urlBuilder = new StringBuilder(apiUrl);
      urlBuilder.append("?" + URLEncoder.encode("ServiceKey","UTF-8") + "="+serviceKey);
      urlBuilder.append("&" + URLEncoder.encode("nx","UTF-8") + "=" + URLEncoder.encode(nx, "UTF-8")); //경도
      urlBuilder.append("&" + URLEncoder.encode("ny","UTF-8") + "=" + URLEncoder.encode(ny, "UTF-8")); //위도
      urlBuilder.append("&" + URLEncoder.encode("base_date","UTF-8") + "=" + URLEncoder.encode(baseDate, "UTF-8")); /* 조회하고싶은 날짜*/
      urlBuilder.append("&" + URLEncoder.encode("base_time","UTF-8") + "=" + URLEncoder.encode(baseTime, "UTF-8")); /* 조회하고싶은 시간 AM 02시부터 3시간 단위 */
      urlBuilder.append("&" + URLEncoder.encode("dataType","UTF-8") + "=" + URLEncoder.encode(type, "UTF-8"));   /* 타입 */
      urlBuilder.append("&" + URLEncoder.encode("numOfRows","UTF-8") + "=" + URLEncoder.encode(numOfRows, "UTF-8"));   /* 한 페이지 결과 수 */
      
      /*
       * GET방식으로 전송해서 파라미터 받아오기
       */
      
      URL url = new URL(urlBuilder.toString());
      

      HttpURLConnection conn = (HttpURLConnection) url.openConnection();
      conn.setRequestMethod("GET");
      conn.setRequestProperty("Content-type", "application/json");
      System.out.println("Response code: " + conn.getResponseCode());
      
      BufferedReader rd;
      if(conn.getResponseCode() >= 200 && conn.getResponseCode() <= 300) {
         rd = new BufferedReader(new InputStreamReader(conn.getInputStream()));
      } else {
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
      for(int i = 0 ; i < parse_item.size(); i++) {
         weather = (JSONObject) parse_item.get(i);
         Object fcstValue = weather.get("fcstValue");
         Object fcstDate = weather.get("fcstDate");
         Object fcstTime = weather.get("fcstTime");
         
         if(fcstValue != null && fcstDate != null && fcstTime != null) {
         //double형으로 받고싶으면 아래내용 주석 해제
         //double fcstValue = Double.parseDouble(weather.get("fcstValue").toString());
            
        	 category = (String)weather.get("category"); 
         
            
            if(category!=null && (category.startsWith("TMP") ||category.startsWith("POP")||category.startsWith("SKY"))) {
            	
            	if(!day.equals(fcstDate.toString())) {
            		day=fcstDate.toString();
            	}
            	
            	if(!time.equals(fcstTime.toString())) {
            		time=fcstTime.toString();
            		System.out.println(day+"  "+time);
            	}
           
            		System.out.print("\tcategory : "+ category);
            		System.out.print(", fcst_Value : "+ fcstValue);
            		System.out.print(", fcstDate : "+ fcstDate);
            		System.out.println(", fcstTime : "+ fcstTime);
            	}
            }
      }
      
      /*
       * 항목값   항목명   단위
       
       * TMP 기온 1시간 씩 나옴
       * POP 강수량
       * SKY 하늘상태(좋음 나쁨 보통) 1 맑음 3 구름많음 4 흐림  2없음
       * 미세먼지 

       */
         
      }
   }
