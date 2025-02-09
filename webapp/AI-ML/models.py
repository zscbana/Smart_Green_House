# models.py
from django.db import models
from django.contrib.auth.models import User

class StudySession(models.Model):
    user = models.ForeignKey(User, on_delete=models.CASCADE)
    duration = models.PositiveIntegerField()  # Duration in seconds
    created_at = models.DateTimeField(auto_now_add=True)

    def __str__(self):
        return f"{self.user.username} - {self.duration // 3600}h {((self.duration % 3600) // 60)}m"
