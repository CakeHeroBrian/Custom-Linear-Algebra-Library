namespace CLAL {
    inline std::array<float, 16> createTransformMatrix(float tx, float ty, float tz, float rx, float ry, float rz, float sx, float sy, float sz) {
  	constexpr float convertToRadians = glm::pi<float>() / 180.0f;
  
  	float radiansR_x = convertToRadians * rx;
  	float radiansR_y = convertToRadians * ry;
  	float radiansR_z = convertToRadians * rz;
  	
  	float a = std::sin(radiansR_x);
  	float b = std::cos(radiansR_x);
  	float c = std::sin(radiansR_y);
  	float d = std::cos(radiansR_y);
  	float e = std::sin(radiansR_z);
  	float f = std::cos(radiansR_z);
  
  	float ac = a * c;
  	float bc = b * c;
  
  	float df = d * f;
  	float de = d * e;
  	float acf_be = ac * f + b * e;
  	float ace_bf = b * f - ac * e;
  	float bcf_ae = a * e - bc * f;
  	float bce_af = bc * e + a * f;
  	float ad = a * d;
  	float bd = b * d;
  
  	return std::array<float, 16>{
  		sx* df, sx* acf_be, sx* bcf_ae, 0.0f,
  			-sy * de, sy* ace_bf, sy* bce_af, 0.0f,
  			sz* c, -sz * ad, sz* bd, 0.0f,
  			tx, ty, tz, 1.0f
  	};
  }
}
