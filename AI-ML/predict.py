import torch
from torchvision import transforms
from PIL import Image
import json
from train import DiseaseClassifier

def load_model(model_path, class_mapping_path, device):
    # Load class mapping
    with open(class_mapping_path, 'r') as f:
        class_to_idx = json.load(f)
    
    # Create reverse mapping
    idx_to_class = {v: k for k, v in class_to_idx.items()}
    
    # Initialize model
    model = DiseaseClassifier(len(class_to_idx)).to(device)
    
    # Load trained weights
    model.load_state_dict(torch.load(model_path, map_location=device))
    model.eval()
    
    return model, idx_to_class

def predict_image(image_path, model, idx_to_class, device):
    # Load and preprocess image
    transform = transforms.Compose([
        transforms.Resize((224, 224)),
        transforms.ToTensor(),
        transforms.Normalize([0.485, 0.456, 0.406], [0.229, 0.224, 0.225])
    ])
    
    image = Image.open(image_path).convert('RGB')
    image_tensor = transform(image).unsqueeze(0).to(device)
    
    # Make prediction
    with torch.no_grad():
        outputs = model(image_tensor)
        probabilities = torch.nn.functional.softmax(outputs, dim=1)
        predicted_class = torch.argmax(probabilities, dim=1).item()
        confidence = probabilities[0][predicted_class].item()
    
    return idx_to_class[predicted_class], confidence

def main():
    # Set device
    device = torch.device('cuda' if torch.cuda.is_available() else 'cpu')
    
    # Load model and class mapping
    model, idx_to_class = load_model('best_model.pth', 'class_mapping.json', device)
    
    # Example prediction
    image_path = input("Enter the path to the image you want to classify: ")
    try:
        predicted_class, confidence = predict_image(image_path, model, idx_to_class, device)
        print(f"\nPrediction: {predicted_class}")
        print(f"Confidence: {confidence:.2%}")
    except Exception as e:
        print(f"Error processing image: {str(e)}")

if __name__ == '__main__':
    main() 